datatype Colour = R | B

datatype 'a RBtree = E | N of Colour * 'a * 'a RBtree * 'a RBtree

(* Dieses lookup funktioniert nur fuer Elemente vom Typ int *)

fun lookup (x,E) = false
  | lookup (x,N(_,y,l,r)) = 
       if x < y then lookup(x,l)
       else if y < x then lookup(x,r)
       else true

fun balance (B,x,N(R,y,N(R,z,t1,t2),t3),t4) =
       N(R,y,N(B,z,t1,t2),N(B,x,t3,t4))
  | balance (B,x,N(R,y,t1,N(R,z,t2,t3)),t4) =
       N(R,z,N(B,y,t1,t2),N(B,x,t3,t4))
  | balance (B,x,t1,N(R,y,N(R,z,t2,t3),t4)) =
       N(R,z,N(B,x,t1,t2),N(B,y,t3,t4))
  | balance (B,x,t1,N(R,y,t2,N(R,z,t3,t4))) =
       N(R,y,N(B,x,t1,t2),N(B,z,t3,t4))
  | balance t = N t

fun insert(x,t) =
   let
      fun ins E = N(R,x,E,E)
        | ins (t as N(c,y,l,r)) = 
             if x < y then balance (c,y,ins l,r)
             else if y < x then balance (c,y,l,ins r)
             else t
      val N(_,y,l,r) = ins t
   in N(B,y,l,r)
   end