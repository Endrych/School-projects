library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
	SMCLK, RESET: in std_logic;
   ROW, LED: out std_logic_vector(0 to 7)
);
end ledc8x8;

architecture main of ledc8x8 is
	signal leds,rows,ce_cnt: std_logic_vector(7 downto 0);
	signal ce: std_logic;
    -- Sem doplnte definice vnitrnich signalu.

begin

    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.
	 
	 ce_gen:process(SMCLK, RESET)
	 begin
		if RESET = '1' then
			ce_cnt <= (others => '0');
		elsif SMCLK'event and SMCLK = '1' then
			ce_cnt <= ce_cnt + 1;
		end if;
	end process ce_gen;
	
	ce <= '1' when ce_cnt = X"FF" else '0';
	
	row_switch:process(RESET, ce, SMCLK)
	begin
		if RESET = '1' then
			rows <= "10000000";
		elsif SMCLK'event and SMCLK = '1' and ce= '1' then
			rows <= rows(0) & rows(7 downto 1);
		end if;
	end process row_switch;
	
	decoder: process(rows)
	begin
		case rows is
				when "10000000" => leds <= "11111111";
				when "01000000" => leds <= "00111111";
				when "00100000" => leds <= "01011111";
				when "00010000" => leds <= "01101000";
				when "00001000" => leds <= "01101011";
				when "00000100" => leds <= "01011000";
				when "00000010" => leds <= "00111011";
				when "00000001" => leds <= "11111000";
				when others =>	null;
		end case;
	end process decoder;

	ROW <= rows;
	LED <= leds;
end main;
