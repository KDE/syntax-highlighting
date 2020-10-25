#!/usr/bin/env lua
-- SPDX-FileCopyrightText: 2020 Jonathan Poelen <jonathan.poelen@gmail.com>
-- SPDX-License-Identifier: MIT

-- load modules or show installation commands
_, argparse = xpcall(require, function(err) trace1 = err end, 'argparse')
_, xmlparser = xpcall(require, function(err) trace2 = err end, 'xmllpegparser')
if not argparse or not xmlparser then
    local versparam = '--lua-version=' .. _VERSION:match('Lua (.*)')
    io.stderr:write('Please install '
                    .. (argparse and '' or 'argparse')
                    .. (not argparse and not xmlparser and ' and ' or '')
                    .. (xmlparser and '' or 'xmllpegparser')
                    .. ':\n\n    luarocks --local ' .. versparam .. ' install'
                    .. (argparse and '' or ' argparse')
                    .. (xmllpegparser and '' or ' xmllpegparser')
                    .. '\n\n    eval "$(luarocks ' .. versparam .. ' path)"\n\n'
                    .. (trace1 or trace2) .. '\n')
    os.exit(1)
end

parser = argparse(arg[0],
                  "Dot file generator for xml syntax\n\nExample:\n    "
                  .. arg[0] .. " data/syntax/lua.xml | dot -T png -o image.png")

function parser:pattern(name, description)
    parser
    :option(name, description .. "\nCan be present several times.")
    :argname'<pattern>'
    :count'*'
end

parser:flag('-c --context-only', "Generates contexts without rules")
parser:flag('-r --replace-entities', "Evaluate html entities")
parser:pattern('-i --include', "Include only contexts that respect a (lua) pattern.")
parser:pattern('-e --exclude', "Exclude contexts that respect a (lua) pattern.")
parser:argument('syntax.xml', "Syntax Definition Files")

do
    -- same message for -h, --help and usage
    local help = parser:get_help()
    parser:usage(help)
    parser:help(help)
end

args = parser:parse()

excludes = args.exclude
-- '' is a pattern always found
includes = #args.include == 0 and {''} or args.include
contextOnly = args.context_only
replaceEntities = args.replace_entities and not contextOnly


do
    -- load file
    local f, err = io.open(args['syntax.xml'])
    if not f then
        io.stderr:write(err .. '\n')
        os.exit(2)
    end

    local content = f:read'*a'

    -- remove BOM
    if content:sub(1,3) == '\xef\xbb\xbf' then
        content = content:sub(4)
    end

    -- load document
    document, err = xmlparser.parse(content)
    if not document then
        io.stderr:write(err .. '\n')
        os.exit(3)
    end
end


local contexts
-- search language/highlighting/contexts
for k,nodes in pairs(document.children[1].children[1].children) do
    if (nodes.tag == 'contexts') then
        contexts = nodes.children
        break
    end
end

if not contexts then
    io.stderr:write('<contexts> not found\n')
    os.exit(4)
end

if replaceEntities then
    local entities = xmlparser.createEntityTable(document.entities)

    for _,ctx in pairs(contexts) do
        for _,rule in pairs(ctx.children) do
            for _,k in pairs({'String', 'char', 'char1'}) do
                if rule.attrs[k] then
                    rule.attrs[k] = xmlparser.replaceEntities(rule.attrs[k], entities)
                end
            end
        end
    end
end


local colorMap = {
    '"/set312/1"',
    '"lightgoldenrod1"',
    '"/set312/3"',
    '"/set312/4"',
    '"/set312/5"',
    '"/set312/6"',
    '"/set312/7"',
    '"/rdpu3/2"',
    '"/rdgy4/3"',
    '"/purd6/3"',
    '"/ylgn4/2"',
    '"/set26/6"',
}

-- returns a color which depends on the first 2 characters
function computeColor(name)
    local hash = name:byte(1)
    if #name ~= 1 then
        hash = hash + name:byte(2) * 25
    end
    return colorMap[hash % #colorMap + 1];
end

function matchPatterns(name, patterns)
    for _,v in pairs(patterns) do
        if name:find(v) then
            return true
        end
    end
    return false
end

function matchContext(name)
    return not matchPatterns(name, excludes) and matchPatterns(name, includes)
end

fmt = string.format

do
    local lpeg = require'lpeg'
    local P = lpeg.P
    local C = lpeg.C
    local S = lpeg.S
    local Cf = lpeg.Cf
    local Cc = lpeg.Cc
    local Cs = lpeg.Cs
    local Ct = lpeg.Ct

    -- #pop counter
    _countpop = Cf((P'#pop' * Cc(1))^1, function(a,b) return a+b end)
    -- cut by slice of 40 characters
    _wordwap = Ct(Cc('') * C(P(40) + P(1)^1)^0)
    -- replace " by \" and \ by \\
    _escape = Cs((S'"' / '\\"' + S'\\' / '\\\\' + 1)^0)
    -- extract context of #pop#pop#pop!Context
    _jumpctx = (1 - S'!')^1 * '!' * C(P(1)^1)
end

function labelize(name)
    local n = _countpop:match(name)
    if n and n > 1 then
        return fmt('#pop(%d)%s', n, name:sub(n * 4 + 1))
    end
    return name
end

-- convert {[k]={data, index}, ...} to {{k, data, index}, ...} sorted by index
-- for stable output
function kIndexedTableToTable(kt)
    local t = {}
    for k,x in pairs(kt) do
        t[#t+1] = {k, x[1], x[2]}
    end
    table.sort(t, function(a,b) return a[3] < b[3] end)
    return t
end

local sharp = string.byte('#',1)


if contextOnly then
    print('digraph G {')
    print('  compound=true;ratio=auto')

    for _,ctx in pairs(contexts) do
        local ctxName = ctx.attrs.name
        if matchContext(ctxName) then
            -- avoid multi arrow for ctx1 -> ctx2
            local kRuleContexts = {}
            for i,rule in ipairs(ctx.children) do
                kRuleContexts[rule.attrs.context or '#stay'] = {i,i}
            end

            if kRuleContexts['#stay'] and kRuleContexts[ctxName] then
                kRuleContexts[ctxName] = nil
            end

            local color = computeColor(ctxName)
            local escapedCtxName = _escape:match(ctxName)
            print(fmt('  "%s" [style=filled,color=%s]', escapedCtxName, color))

            for _,t in pairs(kIndexedTableToTable(kRuleContexts)) do
                local ruleContext, i = t[1], t[2]
                if ruleContext == '#stay' then
                    print(fmt('  "%s" -> "%s" [color=%s]',
                              escapedCtxName, escapedCtxName, color))
                elseif ruleContext:byte(1) == sharp then
                    local escapedRuleContext = _escape:match(ruleContext)
                    local nextContext = _jumpctx:match(escapedRuleContext)
                    print(fmt('  "%s" -> "%s!!%d" [color=%s];\n  "%s!!%d" [label="%s"];',
                              escapedCtxName,
                              escapedCtxName, i, color,
                              escapedCtxName, i, labelize(escapedRuleContext)))
                    if nextContext then
                        print(fmt('  "%s!!%d" -> "%s"',
                                  escapedCtxName, i, nextContext))
                    end
                else
                    print(fmt('  "%s" -> "%s" [color=%s]',
                          escapedCtxName, labelize(_escape:match(ruleContext)), color))
                end
            end
        end
    end

    print('}')

    return 0
end

function stringifyAttrs(t, attrs)
    local attr
    local s = ''
    for k,v in pairs(t) do
        if attrs[v] then
            attr = attrs[v]
            if #attr > 40 then
                attr = table.concat(_wordwap:match(attr),'\n')
            end
            s = s .. '  ' .. v .. ':' .. attr
        end
    end
    return s
end

function xmlBool(s)
    return s == 'true' or s == '1'
end

function printContextAttr(escapedOrigin, escapedCtxName, escapedNameAttr, style, color)
    if not escapedNameAttr then
        print(fmt('    "%s" -> "%s" [style=%s,color=%s];',
                  escapedOrigin, escapedCtxName, style, color))
    elseif escapedNameAttr:byte(1) == sharp then
        print(fmt('    "%s" -> "%s!!%s" [style=%s,color=%s];\n    "%s!!%s" [label="%s",color=%s];',
                  escapedOrigin,
                  escapedCtxName, escapedNameAttr, style, color,
                  escapedCtxName, escapedNameAttr, labelize(escapedNameAttr), color))
    end
end

function printLastTransition(escapedName, escapedCtxName, escapedNameAttr, color)
    if escapedNameAttr:byte(1) == sharp then
        local escapedLastCtx = _jumpctx:match(escapedNameAttr)
        if escapedLastCtx then
            print(fmt('  "%s!!%s" -> "%s" [style=dashed,color=%s];',
                      escapedCtxName, escapedNameAttr, escapedLastCtx, color))
        end
    else
        print(fmt('  "%s" -> "%s" [style=dashed,color=%s];',
                  escapedName, escapedNameAttr, color))
    end
end

local firstLineAttrs = {'attribute','String','char','char1'}
local secondLineAttrs = {'beginRegion','endRegion','lookAhead','firstNonSpace', 'column'}

print('digraph G {')
print('  compound=true;ratio=auto')
for ictx,ctx in pairs(contexts) do
    local ctxName = ctx.attrs.name
    if matchContext(ctxName) then
        local color = computeColor(ctxName)
        local escapedCtxName = _escape:match(ctxName)
        print(fmt('  subgraph cluster%d {', ictx))
        print(fmt('    "%s" [shape=box,style=filled,color=%s];', escapedCtxName, color))

        local name = ctxName
        local escapedName = escapedCtxName
        local kDot = {}
        for irule,rule in pairs(ctx.children) do
            local nextName = ctxName .. '!!' .. irule .. '!!' .. rule.tag
            local escapedNextName = _escape:match(nextName)
            local ruleContext = rule.attrs.context
            print(fmt('    "%s" -> "%s" [style=dashed,color=%s];',
                      escapedName, escapedNextName, color))
            name = nextName
            escapedName = escapedNextName

            local a = ''
            if rule.tag == 'IncludeRules' then
                a = '  ' .. ruleContext
            else
                if not rule.attrs.attribute then
                    rule.attrs.attribute = ctx.attrs.attribute
                end
                a = a .. stringifyAttrs(firstLineAttrs, rule.attrs)
                local a2 = stringifyAttrs(secondLineAttrs, rule.attrs)
                if #a2 ~= 0 then
                    a = a .. '\n' .. a2
                end
            end
            print(fmt('    "%s" [label="%s%s"];', escapedName, rule.tag, _escape:match(a)))

            if xmlBool(rule.attrs.lookAhead) then
                print(fmt('    "%s" [style=dashed];', escapedName))
            end


            if ruleContext == '#stay' then
                print(fmt('    "%s" -> "%s" [color=dodgerblue3];',
                          escapedName, escapedCtxName))
            elseif ruleContext then
                local escapedRuleContext = _escape:match(ruleContext)
                if ruleContext:byte(1) == sharp then
                    local escapedBindCtxName = _jumpctx:match(escapedRuleContext)
                    print(fmt('    "%s" -> "%s!!%s" [color=%s];\n    "%s!!%s" [label="%s"];',
                              escapedName,
                              escapedCtxName, escapedRuleContext, color,
                              escapedCtxName, escapedRuleContext, labelize(escapedRuleContext)))
                    if escapedBindCtxName then
                        kDot[escapedCtxName .. '!!' .. escapedRuleContext .. '!!' .. escapedBindCtxName] = {
                            fmt('  "%s!!%s" -> "%s" [color=%s];\n  "%s!!%s" [color=red];',
                                escapedCtxName, escapedRuleContext,
                                escapedBindCtxName, color,
                                escapedCtxName, escapedRuleContext),
                            irule,
                        }
                    end
                else
                    kDot[irule] = {
                        fmt('  "%s" -> "%s" [color=%s];',
                            escapedName, escapedRuleContext, color),
                        irule,
                    }
                end
            end
        end

        local fallthroughCtx = ctx.attrs.fallthroughContext
        local escapedFallthroughCtx = fallthroughCtx and _escape:match(fallthroughCtx)
        printContextAttr(escapedName, escapedCtxName, escapedFallthroughCtx, 'dashed', color)

        local endCtx = ctx.attrs.lineEndContext
        local escapedEndCtx = endCtx and _escape:match(endCtx)
        printContextAttr(escapedCtxName, escapedCtxName, escapedEndCtx, 'dotted', 'blue')

        print('  }')

        if fallthroughCtx then
            printLastTransition(escapedName, escapedCtxName, escapedFallthroughCtx, color)
        end

        if endCtx then
            printLastTransition(escapedName, escapedCtxName, escapedEndCtx, color)
        end

        for _,t in pairs(kIndexedTableToTable(kDot)) do
            print(t[2])
        end
    end
end
print('}')
