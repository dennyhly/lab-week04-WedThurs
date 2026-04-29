module slave (
    // inputs
    input logic mosi,
    input logic sck,
    // output
    output logic [3:0] led_out,
    // output logic miso
);

logic [3:0] bit_count = 0;
logic [7:0] shift_reg = 0;

always @ (posedge sck) begin
    shift_reg <= {shift_reg[6:0], mosi};
    
    if (bit_count == 7) begin
        led_out <= {shift_reg[6:0], mosi}[7:4];
        bit_count <= 0;
    end
    else begin
        bit_count <= bit_count + 1;
    end
end

endmodule