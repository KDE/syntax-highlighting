--------------------------------------------------------------------------------
-- light52_muldiv.vhdl -- Simple multiplier/divider module.
--------------------------------------------------------------------------------
-- The 8051 mul and div instructions are both unsigned and operands are 8 bit.
--
-- This module implements the division as a sequential state machine which takes
-- 8 cycles to complete. 
-- The multiplier can be implemented as sequential or as combinational, in which
-- case it will use a DSP block in those architectures that support it.
-- No attempt has been made to make this module generic or reusable.
--
-- If you want a combinational multiplier but don't want to waste a DSP block 
-- in this module, you need to modify this file adding whatever synthesis 
-- pragmas your tool of choice needs.
--
-- Note that unlike the division state machine, the combinational product logic
-- is always operating: when SEQUENTIAL_MULTIPLIER=true, prod_out equals 
-- data_a * data_b with a latency of 1 clock cycle, and mul_ready is hardwired
-- to '1'.
--
-- FIXME explain division algorithm.
--------------------------------------------------------------------------------
-- GENERICS:
-- 
-- SEQUENTIAL_MULTIPLIER        -- Sequential vs. combinational multiplier.
--  When true, a sequential implementation will be used for the multiplier, 
--  which will usually save a lot of logic or a dedicated multiplier.
--  When false, a combinational registered multiplier will be used.
--
--------------------------------------------------------------------------------
-- INTERFACE SIGNALS:
--
-- clk :            Clock, active rising edge.
-- reset :          Synchronous reset. Clears only the control registers not
--                  visible to the programmer -- not the output registers.
-- 
-- data_a :         Numerator input, should be connected to the ACC register.
-- data_b :         Denominator input, should be connected to the B register.
-- start :          Assert for 1 cycle to start the division state machine
--                  (and the product if SEQUENTIAL_MULTIPLIER=true);
-- 
-- prod_out :       Product output, valid only when mul_ready='1'.
-- quot_out :       Quotient output, valid only when div_ready='1'.
-- rem_out :        Remainder output, valid only when div_ready='1'.
-- div_ov_out :     Division overflow flag, valid only when div_ready='1'.
-- mul_ov_out :     Product overflow flag, valid only when mul_ready='1'.
-- 
-- mul_ready :      Asserted permanently if SEQUENTIAL_MULTIPLIER=false.
-- div_ready :      Deasserted the cycle after start is asserted.
--                  Asserted when the division has completed.
--
--------------------------------------------------------------------------------
-- Copyright (C) 2012 Jose A. Ruiz
--                                                              
-- This source file may be used and distributed without         
-- restriction provided that this copyright statement is not    
-- removed from the file and that any derivative work contains  
-- the original copyright notice and the associated disclaimer. 
--                                                              
-- This source file is free software; you can redistribute it   
-- and/or modify it under the terms of the GNU Lesser General   
-- Public License as published by the Free Software Foundation; 
-- either version 2.1 of the License, or (at your option) any   
-- later version.                                               
--                                                              
-- This source is distributed in the hope that it will be       
-- useful, but WITHOUT ANY WARRANTY; without even the implied   
-- warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      
-- PURPOSE.  See the GNU Lesser General Public License for more 
-- details.                                                     
--                                                              
-- You should have received a copy of the GNU Lesser General    
-- Public License along with this source; if not, download it   
-- from http://www.opencores.org/lgpl.shtml
--------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.light52_pkg.all;
use work.light52_ucode_pkg.all;

entity light52_muldiv is
    generic (
        SEQUENTIAL_MULTIPLIER : boolean := false
    );
    port(
        clk :                   in std_logic;
        reset :                 in std_logic;

        data_a :                in t_byte;
        data_b :                in t_byte;
        start :                 in std_logic;

        prod_out :              out t_word;
        quot_out :              out t_byte;
        rem_out :               out t_byte;
        div_ov_out :            out std_logic;
        mul_ov_out :            out std_logic;

        mul_ready :             out std_logic;
        div_ready :             out std_logic
    );
end entity light52_muldiv;

architecture sequential of light52_muldiv is

signal bit_ctr :            integer range 0 to 8;

signal b_shift_reg :        t_word;

signal den_ge_256 :         std_logic;
signal num_ge_den :         std_logic;
signal sub_num :            std_logic;

signal denominator :        t_byte;
signal rem_reg :            t_byte;
signal quot_reg :           t_byte;
signal prod_reg :           t_word;
signal ready :              std_logic;

signal load_regs :          std_logic;

begin

-- Control logic ---------------------------------------------------------------

control_counter: process(clk)
    alias sig is <<signal g_test(0).i_test.sig : std_logic>>;
begin
    if clk'event and clk='1' then
        if reset='1' then
            bit_ctr <= 8;
        else
            if load_regs='1' then
                bit_ctr <= 0;
            elsif bit_ctr /= 8 then
                bit_ctr <= bit_ctr + 1;
            end if;
        end if;
    end if;
end process control_counter;

-- Internal signal ready is asserted after 8 cycles.
-- The sequential multiplier will use this signal too, IF it takes 8 cycles.

ready <= '1' when bit_ctr >= 8 else '0';


---- Divider logic -------------------------------------------------------------

-- What we do is a simple base-2 'shift-and-subtract' algorithm that takes
-- 8 cycles to complete. We can get away with this because we deal with unsigned
-- numbers only.

divider_registers: process(clk)
begin
    if clk'event and clk='1' then
        -- denominator shift register
        if load_regs='1' then
            b_shift_reg <= "0" & data_b & "0000000";
            -- Division overflow can be determined upon loading B reg data.
            -- OV will be raised only on div-by-zero.
            if data_b=X"00" then
                div_ov_out <= '1';
            else
                div_ov_out <= '0';
            end if;
        else
            b_shift_reg <= "0" & b_shift_reg(b_shift_reg'high downto 1);
        end if;
        
        -- numerator register
        if load_regs='1' then 
            rem_reg <= data_a;
        elsif bit_ctr/=8 and sub_num='1' then 
            rem_reg <= rem_reg - denominator;
        end if;

        --- quotient register
        if load_regs='1' then
            quot_reg <= (others => '0');
        elsif bit_ctr/=8 then
            quot_reg <= quot_reg(quot_reg'high-1 downto 0) & sub_num;
        end if;
        
        load_regs <= start;
    end if;
end process divider_registers;

denominator <= b_shift_reg(7 downto 0);

-- The 16-bit comparison between b_shift_reg (denominator) and the zero-extended 
-- rem_reg (numerator) can be simplified by splitting it in 2: 
-- If the shifted denominator high byte is not zero, it is >=256...
den_ge_256 <= '1' when b_shift_reg(15 downto 8) /= X"00" else '0';
-- ...otherwise we need to compare the low bytes.
num_ge_den <= '1' when rem_reg >= denominator else '0';
sub_num <= '1' when den_ge_256='0' and num_ge_den='1' else '0';


quot_out <= quot_reg;
prod_out <= prod_reg;
rem_out <= rem_reg;

div_ready <= ready;

---- Multiplier logic ----------------------------------------------------------

---- Combinational multiplier -----------------------------
multiplier_combinational: if not SEQUENTIAL_MULTIPLIER generate

registered_combinational_multiplier:process(clk)
begin
    if clk'event and clk='1' then
        prod_reg <= data_a * data_b; -- t_byte is unsigned
    end if;
end process registered_combinational_multiplier;

-- The multiplier output is valid in the cycle after the operands are loaded,
-- so by the time MUL is executed it's already done.
mul_ready <= '1';

mul_ov_out <= '1' when prod_reg(15 downto 8)/=X"00" else '0';
prod_out <= prod_reg;

end generate multiplier_combinational;

---- Sequential multiplier --------------------------------
multiplier_sequential: if SEQUENTIAL_MULTIPLIER generate

assert false
report "Sequential multiplier implementation not done yet."&
       " Use combinational implementation."
severity failure;

end generate multiplier_sequential;

end sequential;


with Types; use Types;
with Files_Map;

package fixed_pkg is new IEEE.fixed_generic_pkg
  generic map (
    fixed_overflow_style => IEEE.fixed_float_types.fixed_saturate,
    fixed_guard_bits     => 3,
    no_warning           => false
    );

package p is
    type int_ptr is access integer;
    type rec is record
        data  : std_logic_vector(31 downto 0);
        ack   : std_logic;
        value : integer;
        link  : rec_ptr;
    end record;
    type int_vec is array (integer range <>) of integer;
    type int_vec_ptr is access int_vec;
    procedure UNIFORM(variable SEED1, SEED2 : inout POSITIVE; variable X : out REAL);
    constant def_arr : t_int_array := (0 to 2 => 10);

    -- type range
    type newInt is range -4 to 3;
    type CAPACITY is range 0 to 1E5 units
        pF;
        nF = 1000 pF;
    end units;

    -- type protected
    type prot is protected
        function meth(a : int) return bit;
    end protected;

    -- type protected body
    type prot is protected body
        variable var : positive;
        constant const : boolean;

        function meth(a : int) return bit is
        begin
        end function;
    end protected body;

    function \?=\ (L, R  : BOOLEAN) return BOOLEAN;

end package;

package body p is
    function \?=\ (L, R : BOOLEAN) return BOOLEAN is
    begin
        if not (format(format'left) = '%') then
            report "to_string: Illegal format string """ & format & '"'
                severity error;
            return "";
        end if;
        return L = R;
    end function \?=\;

    procedure test is
        variable v : int_ptr;
        variable i : integer;
    begin
        v := null;
        deallocate(v);
        v := new integer;
        v := new integer'(5);
        v.all := 5;
        r.all.value := 1;
        a := new int_vec(1 to 3);
        a.all(5) := 2;
        a(1 to 2) := (1, 2);
        s := new string'("");
    end procedure;

    procedure test2(x : inout rec_ptr) is
    begin
        x.value := x.value + 1;
    end procedure;

    procedure test3 is
        type a;
        type a is access integer;
        variable v : a;
    begin
    end procedure;

    type int_ptr_array is array (integer range <>) of int_ptr;

    procedure tets4 is
        type bvp is access bit_vector;
        variable y : int_ptr(1 to 3) := int_ptr'(null);
    begin
    end procedure;

    procedure Restore_Origin (Mark : Instance_Index_Type) is
    begin
        for I in reverse Mark + 1 .. Prev_Instance_Table.Last loop
            declare
                El : Instance_Entry_Type renames Prev_Instance_Table.Table (I);
            begin
                Origin_Table.Table (El.N) := El.Old_Origin;
            end;
        end loop;
        Prev_Instance_Table.Set_Last (Mark);
    end Restore_Origin;

    --  Instantiate a list.  Simply create a new list and instantiate nodes of
    --  that list.
    function Instantiate_Iir_List (L : Iir_List; Is_Ref : Boolean)
                                    return Iir_List
    is
        Res : Iir_List;
        El : Iir;
    begin
        case to_integer(unsigned(CTRL_REF(x*4+3 downto x*4))) is
            when Null_Iir_List
            | Iir_List_All =>
                return L;
            when others =>
                It := List_Iterate (L);
                while Is_Valid (It) loop
                    El := Get_Element (It);
                    Append_Element (Res, Instantiate_Iir (El, Is_Ref));
                end loop;
                for I in Flist_First .. Flist_Last (L) loop
                    Set_Nth_Element (Res, I, Instantiate_Iir (El, Is_Ref));
                end loop;
                return Res;
        end case;
    end Instantiate_Iir_List;
end package body;

-- Library bar
context foo.test_context;

context foo is
    context foo.test_context;
end context foo;

entity concat is
end entity;

entity foo is
    port (
        x : in my_int );
end entity;

architecture t of concat is
    type int_array is array (integer range <>) of integer;
    type small is range 1 to 3;

    component or_entity is
    port(
        input_1: in std_logic;
        output: out std_logic
        );
    end component;
begin
    process
        variable s : string(1 to 5);
        variable t : int_array(1 to 2);
        variable c : bit_vector(1 to 4);
    begin
        x := ( 1, 2, 3 );
        z := x & y;
        w := 1 & x;
        s := 'h' & string'("ello");
        assert "10" = (b(1) & "0");
        wait;
    end process;

    function CounterVal(Seconds : integer := 0) return integer is
        variable TotalSeconds : interger;
    begin
        TotalSeconds := Seconds + Minutes * 60;
        return TotalSeconds * ClockFrequencyHz -1;
    end function;

    type enum_type is (a, b, c, ..., z);
    type int_array is array(3 downto 0) of integer;

    subtype addr_int is integer range 0 to 65535;
    subtype sub_enum_type is enum_type range a to m;

    inst1: entity work.counter1(rtl)
        generic map (BITS1 => 8)
        port map (
            clk1 => Clock,
            DATA_OUT   => pwm_data_o(3 downto 5),
            COMP_IN(1 downto 0)  => compensate_i,
            WRITE_IN   => (others => '0')
        );

    inst2: component counter2
        generic map (BITS1 => 8)
        port map (clk1 => Clock);

    inst3: configuration counter3
        generic map (BITS1 => 8)
        port map (clk1 => Clock);

    THE_PWM_GEN : pwm_generator
        generic map(
            dsfds => ds
        )
        port map(
            CLK        => clk_i,
            DATA_IN    => pwm_data_i,
            DATA_OUT   => pwm_data_o(3 downto 5),
            COMP_IN(1 downto 0)  => compensate_i,
            WRITE_IN   => (others => '0')
        );

end architecture;

architecture a2 of e is
    function ">"(a, b: my_int) return boolean;
begin
    process is
        variable x, y : my_int;
    begin
        assert x > y;
        assert x < y;                   -- Error
    end process;

    billowitch_tc586: block is
        type real_cons_vector  is array (15 downto 0) of real;
        type real_cons_vector_file is file of real_cons_vector;
        constant C19 : real_cons_vector := (others => 3.0);
    begin
    end block;
end architecture;

architecture arch of ent is
begin
  LL: if test=10 generate
   begin
   end;
  elsif test=5 generate
   begin
   end;
  end generate;

  LL: if l1: SPEED = "fast" generate
  elsif test=5 generate
  end generate;
end architecture arch;


architecture thing_arch of designthing is

component pwm_generator
  port(
    CLK        : in std_logic;
    DATA_IN    : in  std_logic_vector(15 downto 0);
    );
end component pwm_generator;

attribute NOM_FREQ : string;
attribute NOM_FREQ of clk_source : label is "133.00";
signal clk_i  : std_logic;

begin

gen_no_comp: if TEMP = 0 generate
  compensate_i <= (others => '0');
end generate;

gen_no_comp: for i in 0 to TEMP generate
  compensate_i <= (others => '0') after 10 ns;
  compensate_i <= (others => '0') ;
end generate;

---------------------------------------------------------------------------
-- LED blinking when activity on inputs
---------------------------------------------------------------------------
PROC_TIMER : process begin
  wait until rising_edge(clk_i);
  timer <= timer + 1;
  wait for 10 ns;
  leds <= (last_inp xor inp_status(3 downto 0)) or leds or last_leds;
  if timer = 0 then
    leds <= not inp_status(3 downto 0);
    last_leds <= x"0";
  elsif gf then
    fdsa <= '1';
  end if;

  xz: for x in 0 to 7 loop
    dsadf;
  end loop;

  case c is
    when XXX =>
      c <= 1;
      d <= 21321;
    when YYYY =>
      c <= 2;
  end case; 
end process;


generate_with_begin: if TEMP = 0 generate
  signal : test : std_logic;
begin
  compensate_i <= (others => '0');
  if timer = 0 then
    leds <= not inp_status(3 downto 0);
    last_leds <= x"0";
  elsif gf then
    fdsa <= '1';
  end if;  
end generate generate_with_begin;

PROC_TIMER : process
  variable x : std_logic;
begin
  x := '0';
end process PROC_TIMER;

end architecture thing_arc;   --this is not correct (wrong name)

1+1
2ns

1_2_3
12_3
1.2
1.2_3
1_3.2_3
12_3e+1
12_3e-1
12_3e1_1
12_3.4e1_1
12_3e1_
12_3e

2#1_2_3#E+8
2#1_2.3#E+8
2#1_f2.3#

3.14159_26536 -- A literal of type universal_real.
5280          -- A literal of type universal_integer.
10.7 ns       -- A literal of a physical type.
O"4777"       -- A bit string literal.
"54LS281"     -- A string literal.
""            -- A string literal representing a null array.
B"1111_1111_1111" -- Equivalent to the string literal "111111111111".
X"FFF"            -- Equivalent to B"1111_1111_1111".
O"777"            -- Equivalent to B"111_111_111".
X"777"            -- Equivalent to B"0111_0111_0111".
B"XXXX_01LH" -- Equivalent to the string literal "XXXX01LH"
UO"27"       -- Equivalent to B"010_111"
UO"2C"       -- Equivalent to B"011_CCC"
SX"3W"       -- Equivalent to B"0011_WWWW"
D"35"        -- Equivalent to B"100011"
12UB"X1" -- Equivalent to B"0000_0000_00X1"
12SB"X1" -- Equivalent to B"XXXX_XXXX_XXX1"
12UX"F-" -- Equivalent to B"0000_1111_----"
12SX"F-" -- Equivalent to B"1111_1111_----"
12D"13"  -- Equivalent to B"0000_0000_1101"
12UX"000WWW" -- Equivalent to B"WWWW_WWWW_WWWW"
12SX"FFFC00" -- Equivalent to B"1100_0000_0000"
12SX"XXXX00" -- Equivalent to B"XXXX_0000_0000"
8D"511"  -- Error
8UO"477" -- Error
8SX"0FF" -- Error
8SX"FXX" -- Error
