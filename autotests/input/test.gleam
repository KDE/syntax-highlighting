//// Syntax test file for Gleam
////
//// This is a dummy example file and should not be taken as a reference of
//// well written Gleam, but it does compile correctly appart from an error
//// for the absent file "./ffi.mjs" and some warnings.
////
////
////
//// ## Top module documentation comment with markdown formatting
////
//// Formats: _italic text_, **bold text**, `monospace text`, ~~strikeout~~
////
//// Bullet list:
////
//// - item 1
//// - item 2
////
//// Numbered list:
////
//// 1. item 1
//// 2. item 2
////
//// [link](https://example.com)

// SPDX-FileCopyrightText: 2024 Louis Guichard <glpda+kde@pm.me>
// SPDX-License-Identifier: MIT

import gleam/bit_array as bit_string
import gleam/bool
import gleam/dict.{type Dict, delete, get, insert}
import gleam/io

// BEGIN

pub fn main() {
  let assert Ok(prefix) = bit_string.to_string(prefix)
  let name: String = "Kate \u{1F3F3}\u{FE0F}\u{200D}\u{1F308} "
  let assert Ok(suffix) = get(mascots(), "Lucy")
  io.println(prefix <> name <> suffix)
}

const prefix = <<"Hello":utf8, 44:size(8), 32:int>>

fn number(int) {
  use <- bool.guard(when: int < 0, return: False)
  let dec = 123_456_789
  let bin = 0b10011011
  let oct = 0o1234567
  let hex = 0x8337_BEEF
  let _float = -3.1415 *. 1.0e-4
  let roof = { { dec + oct } * bin - hex } / 4
  // 4_246_851_061
  case int {
    _ if int < roof -> True
    _ -> False
  }
}

/// Documentation for a custom type with 3 variants
pub opaque type CustomType(a) {
  /// first variant
  Var1(x: a)

  /// second variant
  Var2(x: a, y: a)

  /// third variant
  Var3(x: a, y: a, z: a)
}

pub fn first(ct: CustomType(a)) -> a {
  ct.x
}

@deprecated("Use last instead")
pub fn third(ct: CustomType(a)) -> a {
  // let assert Var3(_, _, z) = ct  z
  case ct {
    Var3(_, _, z) -> z
    _ -> panic
  }
}

pub fn last(ct: CustomType(a)) -> a {
  case ct {
    Var1(x) -> x
    Var2(_, y) -> y
    Var3(_, _, z) -> z
  }
}

pub type ListError {
  EmptyList
  ListTooLong
}

/// Converts a `List(a)` to a `Result(CustomType(a))`, empty list and list
/// longer than 3 will return an error.
///
/// ## Examples
///
/// ```gleam
/// from_list([1, 2, 3])
/// // -> Ok(Var3(1, 2, 3))
///
/// from_list([1, 2, 3, 4, 5])
/// // -> Error(ListTooLong)
/// ```
///
pub fn from_list(l: List(a)) -> Result(CustomType(a), ListError) {
  case l {
    [x] -> Ok(Var1(x))
    [x, y] -> Ok(Var2(x, y))
    [x, y, z] -> Ok(Var3(x, y, z))
    [] -> Error(EmptyList)
    _ -> Error(ListTooLong)
  }
}

fn mascots() -> Dict(String, String) {
  let old_mascots = dict.from_list([#("Konqi", "🐉"), #("Kandalf", "🧙")])
  let new_mascots =
    old_mascots
    |> delete("Kandalf")
    |> insert("Kate", "🐦")
    |> insert("Katie", "🐉")
    |> insert("Lucy", "🌟")
  new_mascots
}

type UserId =
  Int

@external(javascript, "./ffi.mjs", "call")
fn call(id: UserId) -> String {
  todo as "call not implemented for this target"
}
// END
