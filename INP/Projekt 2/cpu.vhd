-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2017 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='0') / zapis do pameti (DATA_RDWR='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is	
 -- zde dopiste potrebne deklarace signalu
 type type_instr is (
	PTR_INC,
	PTR_DEC,
	CELL_INC,
	CELL_DEC,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	PRINT_VAL,
	READ_VAL,
	BREAK_WHILE,
	END_PROG,
	NOT_INSTRUCTION	
 );
 
 type fsm_state is(
	ST_IDLE,
	ST_FETCH,
	ST_DECODE,
	ST_CELL_INC,
	ST_CELL_DEC,
	ST_PRINT_VAL,
	ST_READ_VAL,
	ST_LEFT_BRACKET,
	ST_LEFT_BRACKET1,
	ST_LEFT_BRACKET2,
	ST_LEFT_BRACKET3,
	ST_LEFT_BRACKET4,
	ST_RIGHT_BRACKET,
	ST_RIGHT_BRACKET1,
	ST_RIGHT_BRACKET2,
	ST_RIGHT_BRACKET3,
	ST_BREAK,
	ST_BREAK1,
	ST_BREAK2,
	ST_BREAK3,
	ST_NFETCH,
	ST_PTR_INC,
	ST_PTR_INC1,
	ST_PTR_DEC,
	ST_PTR_DEC1,
	ST_HALT
 );
 
 signal pstate : fsm_state;
 signal nstate : fsm_state;
 signal pc_reg : std_logic_vector(11 downto 0);
 signal pc_dec : std_logic;
 signal pc_inc : std_logic;
 signal cnt_reg : std_logic_vector(7 downto 0);
 signal cnt_inc : std_logic;
 signal cnt_dec : std_logic;
 signal ptr_reg : std_logic_vector(9 downto 0);
 signal ptr_reg_inc : std_logic;
 signal ptr_reg_dec : std_logic;
 signal instruction : type_instr;
 signal data_select: std_logic_vector(1 downto 0);

begin

 -- zde dopiste vlastni VHDL kod
 DATA_ADDR <= ptr_reg;
 CODE_ADDR <= pc_reg;
 
 -- RADIC
 nsl: process (pstate, CODE_DATA, DATA_RDATA, IN_DATA, IN_VLD, OUT_BUSY, instruction, cnt_reg, data_select)
 begin
 -- INIT
 nstate <= ST_IDLE;
 CODE_EN <= '0';
 DATA_EN <= '0';
 IN_REQ <= '0';
 OUT_WE <= '0';
 cnt_inc <= '0';
 cnt_dec <= '0';
 pc_dec <= '0';
 pc_inc <= '0';
 ptr_reg_inc <= '0';
 ptr_reg_dec <= '0';
 data_select <= "11";
 DATA_RDWR <= '0';
 OUT_DATA <= (others => '0');
 
	case pstate is
		-- IDLE
		when ST_IDLE =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= ST_FETCH;
		when ST_FETCH =>
			nstate <= ST_DECODE;
			CODE_EN <= '1';
		when ST_DECODE =>
			case instruction is 
				when CELL_INC =>
					nstate <= ST_CELL_INC;
				when PRINT_VAL =>
					nstate <= ST_PRINT_VAL;
				when END_PROG =>
					nstate <= ST_HALT;
				when CELL_DEC =>
					nstate <= ST_CELL_DEC;
				when PTR_INC => 
					nstate <= ST_PTR_INC;
				when PTR_DEC => 
					nstate <= ST_PTR_DEC;
				when READ_VAL =>
					nstate <= ST_READ_VAL;
				when LEFT_BRACKET =>
					nstate <= ST_LEFT_BRACKET;
				when RIGHT_BRACKET =>
					nstate <= ST_RIGHT_BRACKET;
				when BREAK_WHILE =>
					nstate <= ST_BREAK;
				when others => 
					nstate <= ST_NFETCH;
			end case;
		when ST_CELL_INC =>
			DATA_RDWR <= '1';
			data_select <= "10";
			DATA_EN <= '1';
			nstate <= ST_NFETCH;
		when ST_CELL_DEC =>
			DATA_RDWR <= '1';
			DATA_EN <= '1';
			data_select <= "01";
			nstate <= ST_NFETCH;
		when ST_PTR_INC =>
			ptr_reg_inc <= '1';
			nstate <= ST_PTR_INC1;
		when ST_PTR_INC1 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= ST_NFETCH;
		when ST_PTR_DEC =>
			ptr_reg_dec <= '1';
			nstate <= ST_PTR_DEC1;
		when ST_PTR_DEC1 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= ST_NFETCH;
		when ST_PRINT_VAL =>
			if(OUT_BUSY = '1') then
				nstate <= ST_PRINT_VAL;
			else
				OUT_WE <= '1';
				OUT_DATA <= DATA_RDATA;
				nstate <= ST_NFETCH;
			end if;
		when ST_READ_VAL =>
			if (IN_VLD  = '1') then
				DATA_RDWR <= '1';
				DATA_EN <= '1';
				data_select <= "00";
				nstate <= ST_NFETCH;
			else
				IN_REQ <= '1';
				nstate <= ST_READ_VAL;
			end if;
		when ST_LEFT_BRACKET =>
			DATA_RDWR <= '0';
			DATA_EN <= '1';
			nstate <= ST_LEFT_BRACKET1;	
		when ST_LEFT_BRACKET1 =>
			if(DATA_RDATA = "00000000")then
				cnt_inc <= '1';
				pc_inc <= '1';
				CODE_EN <= '1';
				nstate <= ST_LEFT_BRACKET2;
			else
				nstate <= ST_NFETCH;
			end if;
		when ST_LEFT_BRACKET2 =>
			CODE_EN <= '1';
			nstate <= ST_LEFT_BRACKET3;
		when ST_LEFT_BRACKET3=>
			if(instruction = LEFT_BRACKET)then
				cnt_inc <= '1';
				pc_inc <= '1';
				nstate <= ST_LEFT_BRACKET2;
			elsif(instruction = RIGHT_BRACKET)then
				cnt_dec <= '1';
				nstate <= ST_LEFT_BRACKET4;
			else
				pc_inc <= '1';
				nstate <= ST_LEFT_BRACKET2;
			end if;
		when ST_LEFT_BRACKET4 =>
			if(cnt_reg = "00000000")then
				nstate <= ST_NFETCH;
			else
				pc_inc <= '1';
				nstate <= ST_LEFT_BRACKET2;
			end if;
		when ST_RIGHT_BRACKET =>
			DATA_RDWR <= '0';
			DATA_EN <= '1';
			nstate <= ST_RIGHT_BRACKET1;
		when ST_RIGHT_BRACKET1 =>
			if( DATA_RDATA = "00000000")then
				nstate <= ST_NFETCH;
			else
				cnt_inc <= '1';
				pc_dec <= '1';
				nstate <= ST_RIGHT_BRACKET2;
			end if;
		when ST_RIGHT_BRACKET2 =>
			if(cnt_reg = "00000000")then
				nstate <= ST_NFETCH;
			else
				CODE_EN <= '1';
				nstate <= ST_RIGHT_BRACKET3;
			end if;
		when ST_RIGHT_BRACKET3 =>
			if(instruction = LEFT_BRACKET)then
				cnt_dec <= '1';
			elsif(instruction = RIGHT_BRACKET)then
				cnt_inc <= '1';
			end if;
			if(cnt_reg = "00000000")then				
				pc_inc <= '1';
			else
				pc_dec <= '1';
			end if;
			nstate <= ST_RIGHT_BRACKET2;
		when ST_BREAK =>
			cnt_inc <= '1';
			pc_inc <= '1';
			nstate <= ST_BREAK1;
		when ST_BREAK1 =>
			CODE_EN <= '1';
			nstate <= ST_BREAK2;
		when ST_BREAK2 =>
			if(instruction = LEFT_BRACKET)then
				cnt_inc <= '1';
				pc_inc <= '1';
				nstate <= ST_BREAK1;
			elsif(instruction = RIGHT_BRACKET)then
				cnt_dec <= '1';
				nstate <= ST_BREAK3;
			else
				pc_inc <= '1';
				nstate <= ST_BREAK1;
			end if;
		when ST_BREAK3 =>
			if(cnt_reg = "00000000")then
				nstate <= ST_NFETCH;
			else
				pc_inc <= '1';
				nstate <= ST_BREAK1;
			end if;
		when ST_NFETCH =>
			nstate <= ST_FETCH;
			pc_inc <= '1';
		when ST_HALT =>
			nstate <= ST_HALT;
		when others =>
			nstate <= ST_IDLE;
	end case;
 end process nsl;
 
 -- PROGRAMOVY CITAC
 pc_cntr: process (RESET, CLK)
 begin
	if (RESET='1') then
		pc_reg <= (others=>'0');
	elsif (CLK'event) and (CLK='1') then
		if (pc_dec='1') then
			pc_reg <= pc_reg - 1;
		elsif (pc_inc='1') then
			pc_reg <= pc_reg + 1;
		end if;
	end if;
 end process pc_cntr;
 
 -- CNT REGISTR
 cnt_cntr: process (RESET, CLK)
 begin
	if (RESET='1') then
		cnt_reg <= (others=>'0');
	elsif (CLK'event) and (CLK='1') then
		if (cnt_dec='1') then
			cnt_reg <= cnt_reg - 1;
		elsif (cnt_inc='1') then
			cnt_reg <= cnt_reg + 1;
		end if;
	end if;
 end process cnt_cntr;
 
 -- UKAZATEL DO PAMETI DAT
 ptr_cntr: process (RESET, CLK)
 begin
	if (RESET='1') then
		ptr_reg <= (others=>'0');
	elsif (CLK'event) and (CLK='1') then
		if (ptr_reg_dec='1') then
			ptr_reg <= ptr_reg - 1;
		elsif (ptr_reg_inc='1') then
			ptr_reg <= ptr_reg + 1;
		end if;
	end if;
 end process ptr_cntr;
 
 fsm_pstate: process (RESET,CLK)
 begin 
	if (RESET='1') then
		pstate <= ST_IDLE;
	elsif (CLK'event) and (CLK='1') then
		if(EN='1') then
			pstate <= nstate;
		end if;
	end if;
end process;
 
 -- Dekoder instrukci
 decoder: process(CODE_DATA)
 begin
	case CODE_DATA is
		when X"3E" =>	instruction <= PTR_INC;
		when X"3C" =>	instruction <= PTR_DEC;
		when X"2B" =>	instruction <= CELL_INC;
		when X"2D" =>	instruction <= CELL_DEC;
		when X"5B" =>	instruction <= LEFT_BRACKET;
		when X"5D" =>	instruction <= RIGHT_BRACKET;
		when X"2E" =>	instruction <= PRINT_VAL;
		when X"2C" =>	instruction <= READ_VAL;
		when X"7E" =>	instruction <= BREAK_WHILE;
		when X"00" =>	instruction <= END_PROG;
		when others => instruction <= NOT_INSTRUCTION;
	end case;
 end process decoder;
 
 data_mx: process(data_select, IN_DATA, DATA_RDATA)
 begin
	case(data_select) is 
		when "00" => DATA_WDATA <= IN_DATA;
		when "01" => DATA_WDATA <= DATA_RDATA - 1;
		when "10" => DATA_WDATA <= DATA_RDATA + 1;
		when "11" => DATA_WDATA <= DATA_RDATA;
		when others => null;
	end case;
 end process data_mx;

end behavioral;
 
