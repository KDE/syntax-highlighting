with Ada.Containers.Vectors;
with Ada.Strings;  use Ada.Strings;
with Put_Title;

procedure LAL_DDA is
    Collection : Repinfo_Collection;

    A_Basic_Record       : Basic_Record         := Basic_Record'(A => 42);
    Another_Basic_Record : Basic_Record         := (A => 42);
    Nix                  : constant Null_Record := (null record);

    procedure Process_Type_Decl (Decl : Base_Type_Decl);
    --  Display all representation information that is available in
    --  ``Collection`` for this type declaration.

    procedure Process_Variants
      (Variants : Variant_Representation_Array; Prefix : String);
    --  Display all representation information for the given record variants.
    --  ``Prefix`` is used as a prefix for all printed lines.

    package Expr_Vectors is new Ada.Containers.Vectors (Positive, Expr);
    use type Expr_Vectors.Vector;
    package Expr_Vector_Vectors is new Ada.Containers.Vectors
      (Positive, Expr_Vectors.Vector);

    function Test_Discriminants
      (Decl : Base_Type_Decl) return Expr_Vector_Vectors.Vector;
    --  Fetch the vector of discriminants to use for testing from nearby Test
    --  pragmas.

    procedure Error (Node : Ada_Node'Class; Message : String) with No_Return;
    --  Abort the App with the given error ``Message``, contextualized using
    --  ``Node`` 's source location.

    package App is new Libadalang.Helpers.App
      (Name         => "lal_dda",
       Description  =>
         "Exercize Libadalang's Data_Decomposition API on type declarations",
       App_Setup    => App_Setup,
       Process_Unit => Process_Unit);

    package Args is
        use GNATCOLL.Opt_Parse;

        package Rep_Info_Files is new Parse_Option_List
          (App.Args.Parser, "-i", "--rep-info-file",
           Arg_Type   => Unbounded_String,
           Accumulate => True,
           Help       => "Output for the compiler's -gnatR4j option");

    end Args;

    ---------------
    -- App_Setup --
    ---------------

    procedure App_Setup (Context : App_Context; Jobs : App_Job_Context_Array) is
        pragma Unreferenced (Context, Jobs);
    begin
        Collection := Load (Filename_Array (Args.Rep_Info_Files.Get));
    exception
        when Exc : Loading_Error =>
            Put_Line
              ("Loading_Error raised while loading representation information:");
            Put_Line (Exception_Message (Exc));
            New_Line;
    end App_Setup;

    ------------------
    -- Process_Unit --
    ------------------

    procedure Process_Unit (Context : App_Job_Context; Unit : Analysis_Unit) is
        pragma Unreferenced (Context);

        function Process (Node : Ada_Node'Class) return Visit_Status;

        function Process (Node : Ada_Node'Class) return Visit_Status is
        begin
            case Node.Kind is
                when Ada_Base_Type_Decl =>
                    Process_Type_Decl (Node.As_Base_Type_Decl);

                when Ada_Pragma_Node =>
                    declare
                        PN   : constant Pragma_Node := Node.As_Pragma_Node;
                        Name : constant Text_Type := To_Lower (PN.F_Id.Text);
                        Decl : Ada_Node;
                    begin
                        if Name = "test_object_type" then
                            Decl := PN.Previous_Sibling;
                            if Decl.Kind /= Ada_Object_Decl then
                                Error
                                  (Node,
                                    "previous declaration must be an object"
                                    & " declaration");
                            end if;
                            Process_Type_Decl
                              (Decl.As_Object_Decl
                                .F_Type_Expr
                                .P_Designated_Type_Decl);
                        end if;
                        if I > 1 then
                            Put (", ");
                        end if;
                    end;

                when others =>
                    null;
            end case;
            return Into;
        end Process;

    begin
        Put_Title
          ('#', "Analyzing " & Ada.Directories.Simple_Name (Unit.Get_Filename));
        if Unit.Has_Diagnostics then
            for D of Unit.Diagnostics loop
                Put_Line (Unit.Format_GNU_Diagnostic (D));
            end loop;
            New_Line;
            return;

        elsif not Unit.Root.Is_Null then
            Unit.Root.Traverse (Process'Access);
        end if;
    end Process_Unit;
end LAL_DDA;

type Car is record
    Identity       : Long_Long_Integer;
    Number_Wheels  : Positive range 1 .. 16#FF#E1;
    Number_Wheels  : Positive range 16#F.FF#E+2 .. 2#1111_1111#;
    Paint          : Color;
    Horse_Power_kW : Float range 0.0 .. 2_000.0;
    Consumption    : Float range 0.0 .. 100.0;
end record;

type Null_Record is null record;

type Traffic_Light_Access is access Mutable_Variant_Record;
Any_Traffic_Light       : Traffic_Light_Access :=
                           new Mutable_Variant_Record;
Aliased_Traffic_Light   : aliased Mutable_Variant_Record;

pragma Unchecked_Union (Union);
pragma Convention (C, Union);    -- optional

type Programmer is new Person
                   and Printable
with
   record
      Skilled_In : Language_List;
   end record;

3#12.112#e3
3#12.11 use
--     ^ invalid
3#12.23#e3
--    ^ invalid
3#12.11ds#
--     ^ invalid
1211ds
--  ^ invalid
