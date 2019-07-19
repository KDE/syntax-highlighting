/// <reference types="node" />

// TypeScript Test

class Student {
    fullName: string;
    constructor(public firstName: string, public middleInitial: string, public lastName: string) {
        this.fullName = firstName + " " + middleInitial + " " + lastName;
    }
}

interface Person {
    firstName: string;
    lastName: string;
}

function greeter(person : Person) {
    return "Hello, " + person.firstName + " " + person.lastName;
}

let user = new Student("Jane", "M.", "User");

document.body.innerHTML = greeter(user);
JSON.stringify()
console.log("Hello world");

import http = require("http");
import path = require("path");
import URL = url.URL;
import { Readable, Writable } from "stream";
import { isBuffer, isString, isObject } from "util";

const port = 8__88___8;
const baseUrl = new URL(`http://localhost:${port}/`);
const rootDir = path.dirname(__dirname);

const defaultBrowser = os.platform() === "win32" ? "edge" : "chrome";
let browser: "edge" | "chrome" | "none" = defaultBrowser;
let grep: string | undefined;

interface FileBasedTestConfiguration {
    [setting: string]: string;
}

function swapCase(s: string): string {
    return s.replace(/\w/g, (ch) => {
        const up = ch.toUpperCase();
        return ch === up ? ch.toLowerCase() : up;
    });
}

for (var i in pieces) {
    switch (true) {
        case /^\"?Accession\"?/.test(pieces[i]):
            numeration[0] = i;
            break;
    }
}

const enum ResponseCode {
    Success = 200,
    BadRequest = 400
}

// Substitutions
export function isInOpenInterval(start: number, end: number) {
    return tersible(a => a > start && a < end, () => `(${start}...${end})`)
}
const bar = `${x} ${y}`; 

// readonly
function f1(mt: [number, number], rt: readonly [number, number]) {
}

function f2(ma: string[], ra: readonly string[], mt: [string, string], rt: readonly [string, string]) {
}

type ReadWrite<T> = { -readonly [P in keyof T] : T[P] };

// const assertion
let obj = { x: 10, y: [20, 30], z: { a: { b: 42 } } } as const;
let r2 = { x: 20, y: 10 } as const;
let r2 = {...p} as const;
let p1 = { x: 10, y: 20 as const };
let p2 = { 10 as const, 'foo' as const };

// Definite assignment operator
class Cl {
    one?: string;
    two!: string;
}
let x! = 1; 

// Function with multiple arguments
const children = [].map<Set<Map<number, string>>>(element => {
    if (!this.identityProvider) {
        return element;
    }
    return element;
}); 

// Numerics
var a = 0xA;
var b = 0b1;
var c = 0o7;
var d = 1.1E+3;
var e = 1.E+3;
var f = .1E+3;
var g = 1E+3;
var h = 1.1;
var i = 1.;
var j = .1;
var k =  1;
var l = 1__.e+3_22 | .2____e2 | 0o1_23 | 11__. ;

// Bigint
const binBig = 0b101n;
const octBig = 0o567n;
const hexBig = 0xC0Bn;
const decBig = 123n;

// Types
let a: null = null;
let b: number = 123;
let c: number = 123.456;
let d: string = `Geeks`;
let e: undefined = undefined;
let f: boolean = true;
let g: number = 0b111001; // Binary
let h: number = 0o436; // Octal
let i: number = 0xadf0d; // Hexa-Decimal 

const query = query<[number], number>(`
        SELECT *
        FROM statistics
        WHERE unit_id = $1`)

function runQuery() {
    const query = createQuery<[number[]], Table<Columns>>(`
        some SQL here
    `)
    return database.execute(query)
}

aa: <sdf/> string ?<ssd/> string 
 string // Don't highlight
aa: string assa |
   string 
   string ;
   string

aa: { string
    string } // Don't highlight
    
aa: [ string
      string ]
aa: ( string 
      string ) // Don't highlight
aa: string <string>
interface a {
    aa: /* comment
        */ string,
    bb: /* comment */
        number,
    cc: // comment
        void,
    dd:
        any,
}

null, <{[key]: () => any}> null
null, <{[key]: () =>{a: number}}> null

// Correctly highlighting regular expressions
dt = ((dt[0] * 1e9 + dt[1]) / 1e6).toFixed(3).replace(/\.?0+$/, '') + 'ms';
(a[0] / 2) / (2)

// Type guards
function isNumber(x: any): x is number {
    return typeof x === "number";
}

// Conditional expressions (highlight keywords before ":")
class C {
    w = () =>
        this.a() ? true : this.b() ? false : true;
    z = () =>
        this.b() ? hello : k; 
}
