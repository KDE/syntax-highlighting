* Current Mode PWM Average Model
*
.Subckt PWM-CM nA nC nP nVCtrl nD Params: Lsw=100uH Fsw=100kHz DCMax=0.95 DCMin=0.05 RISense=1 Slope=100m
*
* Input Resistor on Control Pin
RVCtrl nVCtrl 0 1Meg
*
* ON Duty Cycle
EDuty nD 0 Value={V(nDPC)*V(nCx,nP)/(V(nA,nP)-V(nCx,nP)+1u)} ; EDuty
*
.EndS PWM-CM

** Includes
.include some_model.mod
.include models/test/BC550.mod


** Comments
* General SPICE.
; PSpice
$ ngspice
.param ... ; PSpice
.param ... $ ngspice

** Parameters
.param TESTPARAM = '2 * 1.5V'
.param TESTPARAM = "2 * 1.5V"
.param TESTPARAM = { 2 * 1.5V }
.param TESTPARAM = [ 2 * 1.5V ]
.param TESTPARAM = ( 2 * 1.5V )

.four 1Meg v(input)

Vinput input 0 SIN(500mV 1V 1k)
vinput input 0 sin(500mV 1V 1k)

* Input Sources
EXP(0 2V 1ms 2ms 3ms 1 2)
exp(0 2V 1ms 2ms 3ms 1 2)
* lx is also a function
Lx node1 node2 200mH

** Sub-Circuit
* Call
X1 inv ninv out v+ v- LM324
x1 inv ninv out v+ v- lm324

.end
