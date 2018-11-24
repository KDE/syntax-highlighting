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
