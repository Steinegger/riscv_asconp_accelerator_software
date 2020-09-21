import sys
from intelhex import IntelHex

ih = IntelHex()
ih.loadhex(sys.argv[1])


singleROMsize = 8192
totalMemoryLocations = (4 * singleROMsize)

prefix = """`timescale 1ns/1ps

module CustomRom_8192X32
#(
	parameter ADDRESS_WIDTH  = 16,
	parameter ROM_ARRAY_PATH = ""
	)
(
		input logic 					CK,
		input logic						CSB,
		input logic	[ADDRESS_WIDTH-1:0]				A,
		output logic [31:0]		DO
);
localparam  ROMS = 8;

logic [ADDRESS_WIDTH-1:0] address_q;
"""

newRom = """const logic [0:8191] [31:0] ROM_TABLE_{} = {{\n"""

suffix = """};

always_ff @(posedge CK) begin
	if (CSB == 1'b1) begin
		address_q <= A;
	end
end

// assign DO = ROM_TABLE[address_q];
always_comb begin
	unique case (address_q[ADDRESS_WIDTH-2:13])
		2'b00: DO = ROM_TABLE_0[address_q[12:0]];
		2'b01: DO = ROM_TABLE_1[address_q[12:0]];
		2'b10: DO = ROM_TABLE_2[address_q[12:0]];
		2'b11: DO = ROM_TABLE_3[address_q[12:0]];
		// 3'b100: DO = ROM_TABLE_4[address_q[12:0]];
		// 3'b101: DO = ROM_TABLE_5[address_q[12:0]];
		// 3'b110: DO = ROM_TABLE_6[address_q[12:0]];
		// 3'b111: DO = ROM_TABLE_7[address_q[12:0]];
	endcase
end

endmodule
"""

verilog_out_path = sys.argv[2]

d = ih.todict()

def linetoverilog(totalLines, linestring):
    ROMindex = int(totalLines // singleROMsize)

    myStr = ""

    if ((totalLines % singleROMsize) != 0):
        myStr += ",\n"

    else:
        # if not the first line in total close the brackets first
        if (totalLines != 0):
            myStr += "\n};\n\n"
        myStr += newRom.format(ROMindex)

    myStr += linestring

    return myStr


with open(verilog_out_path, 'w') as out:
    out.write(prefix + '\n')

    c = []
    cnt = 0
    lines = 0
    for k,v in d.items():
      c.append(v)
      cnt += 1
      if cnt == 4:
        cnt = 0
        hex_str = "{:02x}{:02x}{:02x}{:02x}".format(c[0],c[1],c[2],c[3])
        hex_int = int(hex_str, 16)
        bin_str = format(hex_int, '0>32b')
        out.write(linetoverilog(lines, " 32'b" + bin_str))

        print(bin_str)
        c = []
        lines += 1

    for i in range(totalMemoryLocations - lines):
        out.write(linetoverilog(i + lines, " 32'b" + 32 * '0'))
        print(32 * '0')


    out.write(suffix + '\n')
