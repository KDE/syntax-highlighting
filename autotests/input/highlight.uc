/**
* Comment 
*/
class DH_SeqAvt_GiveExp extends SequenceAction;

var() float expGive;
var float result;

// line comment
event Activated()
{
  local DH_PlayerController PC;
  PC = DH_PlayerController(GetWorldInfo().GetALocalPlayerController());
  PC.modifyExp(expGive);
  result = PC.currentEXP;
}

defaultproperties
{
  ObjName="Reward Exp"
  ObjCategory="DH_PlayerController"

  VariableLinks.Empty;
  VariableLinks(0)=(ExpectedType=class’SeqVar_Float’, bWriteable=false, LinkDesc="expGive", PropertyName=expGive)
}
