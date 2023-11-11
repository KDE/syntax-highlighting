<#
  .SYNOPSIS
  Lorem ipsum dolor sit amet.

  .DESCRIPTION
  Lorem ipsum dolor sit amet, consectetur adipiscing elit,
  sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.

  .PARAMETER InputPath
  Lorem ipsum dolor sit amet, consectetur adipiscing elit.

  .parameter InputPath
  Lorem ipsum dolor sit amet, consectetur adipiscing elit.

  bla .PARAMETER InputPath
  Lorem ipsum dolor sit amet, consectetur adipiscing elit.

  .EXAMPLE
  PS> .\foo.ps1
#>

@" multi
   line
   string
   $x
   "'
   \n
 "@
"@

@' multi
   line
   string
   $x
   "'
   \n
 '@
'@

$foo = "Test `u{2013} String`nwith `"escapes`" """
$foo = "Test `u{2013} String`nwith `"escapes`" `
dssad"
$foo = 'bla bla''bla bla $x `n'
$Global:HOME = "abc"

[bool]::Parse('false')

echo [bool]::Parse('false')
echo $env:EDITOR ${env:EDITOR} $foo ${foo} $foo-bar ${foo-bar} ${a b
c}
echo "hashtable: $(@{ key = 'value' })"
echo 122.42kb 332.d 23d 625 3232e+2 0x233 0b1101
echo 0xaj 0b1112 123a

docker run -dp 3000:3000 `
  -w /app -v "$(pwd):/app" `
  --network todo-app `
  -e MYSQL_HOST=mysql `
  -e MYSQL_USER=root `
  -e MYSQL_PASSWORD=secret `
  -e MYSQL_DB=todos `
  node:12-alpine `
  cmd "npm install && npm run start"


function Get-NewPix
{
  $start = Get-Date -Month 1 -Day 1 -Year 2010
  $allpix = Get-ChildItem -Path $env:UserProfile\*.jpg -Recurse
  $allpix | Where-Object {$_.LastWriteTime -gt $Start}
}

function Get-SmallFiles {
  Param($Size)
  Get-ChildItem $HOME | Where-Object {
    $_.Length -lt $Size -and !$_.PSIsContainer
  }
}

function Get-EscapedPath
{
    param(
    [Parameter(
        Position=0,
        Mandatory=$true
        ValueFromPipeline=$true,
        ValueFromPipelineByPropertyName=$true)
    ]
    [string]$path
    )

    process {
        if ($path.Contains(' '))
        {
            return '"' + $path + '"'
        }
        return $path
    }
}

<#
   Copied from Craft for testing syntax highlighting
#>

#    this file sets some environment variables that are needed
#    for finding programs and libraries etc.
#    by Hannah von Reth <vonreth@kde.org>
#    you should copy kdesettings.ini to ..\etc\kdesettings.ini
#    and adapt it to your needs (see that file for more info)

#    this file should contain all path settings - and provide thus an environment
#    to build and run kde programs
#    based on kdeenv.bat

cls


$env:CraftRoot=[System.IO.Path]::GetDirectoryName($myInvocation.MyCommand.Definition)

$CRAFT_ARGUMENTS = $args

&{
[version]$minPythonVersion = 3.6

function findPython([string] $name)
{
    $py = (Get-Command $name -ErrorAction SilentlyContinue)
    if ($py -and ($py | Get-Member Version) -and $py.Version -ge $minPythonVersion) {
        $env:CRAFT_PYTHON=$py.Source
    }
}

findPython("python3")
findPython("python")

function readINI([string] $fileName)
{
   $ini = @{}

  switch -regex -file $fileName {
    "^\[(.+)\]$" {
      $section = $matches[1].Trim()
      $ini[$section] = @{}
    }
    "^\s*([^#].+?)\s*=\s*(.*)" {
      $name,$value = $matches[1..2]
      $ini[$section][$name] = $value.Trim()
    }
  }
  $ini
}



if(test-path -path $env:CraftRoot\..\etc\kdesettings.ini)
{
    $settings = readINI $env:CraftRoot\..\etc\kdesettings.ini
}
else
{
    Write-Error("$env:CraftRoot\..\etc\kdesettings.ini Does not exist")
    break
}
if( $CRAFT_ARGUMENTS[0] -eq "--get")
{
    Write-Host($settings[$CRAFT_ARGUMENTS[1]][$CRAFT_ARGUMENTS[2]])
    break
}


function prependPATH([string] $path)
{
    $env:PATH="$path{0}$env:PATH" -f [IO.Path]::PathSeparator
}

if( -Not $env:CRAFT_PYTHON)
{
    prependPATH $settings["Paths"]["Python"]
    $env:CRAFT_PYTHON=[IO.PATH]::COMBINE($settings["Paths"]["Python"], "python")
}

(& $env:CRAFT_PYTHON ([IO.PATH]::COMBINE("$env:CraftRoot", "bin", "CraftSetupHelper.py")) "--setup") |
foreach {
  if ($_ -match "=") {
    $v = $_.split("=")
    set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
    #Write-Host("$v[0]=$v[1]")
  }
}

cd "$env:KDEROOT"
}


function Global:craft() {
    return & $env:CRAFT_PYTHON ([IO.PATH]::COMBINE("$env:CraftRoot", "bin", "craft.py")) $args
}


if($args.Length -ne 0)
{
    invoke-expression -command "$args"
}
