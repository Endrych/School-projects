import IPPInterpret
import XMLParser
import sys
import argparse

argparser = argparse.ArgumentParser()
argparser.add_argument('--source', '-s', type=str, dest="source")
args = argparser.parse_args()
if args.source != None:
    file = open(args.source, "r")
else:
    file = sys.stdin

parser = XMLParser.XMLParser(file)
program = parser.parse_document()
interpret = IPPInterpret.IPPInterpret(program)
interpret.interpret_program()