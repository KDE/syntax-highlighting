# This file is a testcase for the highlighting of Crystal code
# It's not executable code, but a collection of code snippets
#

require "lib_z"
require "./digest"

module Digest::Adler32
  def self.initial : UInt32
    LibZ.adler32(0, nil, 0).to_u32
  end

  def self.checksum(data) : UInt32
    update(data, initial)
  end

  def self.update(data, adler32 : UInt32) : UInt32
    slice = data.to_slice
    LibZ.adler32(adler32, slice, slice.size).to_u32
  end

  def self.combine(adler1 : UInt32, adler2 : UInt32, len) : UInt32
    LibZ.adler32_combine(adler1, adler2, len).to_u32
  end
end

struct BigRational
  Number.expand_div [Int8, UInt8, Int16, UInt16, Int32, UInt32, Int64, UInt64, Int128, UInt128], BigRational
  Number.expand_div [Float32, Float64], BigRational
end

module Crystal::Repl::Closure
  VAR_NAME = ".closure_var"
  ARG_NAME = ".closure_arg"
end

class FunctionType
  getter arg_types : Array(ArgType)
  getter return_type : ArgType

  @@cvar = 3
  @ivar = 7

  def initialize(@arg_types, @return_type)
  end

  def //(other : Int::Unsigned) : BigInt
    check_division_by_zero other
    unsafe_floored_div(other)
  end

  def //(other : Int) : BigInt
    check_division_by_zero other

    if other < 0
      (-self).unsafe_floored_div(-other)
      @cvar += 1
    else
      unsafe_floored_div(other)
     @ivar += 10
    end
  end
end

require "llvm/enums/atomic"
struct Atomic(T)
  # Creates an Atomic with the given initial value.
  def compare_and_set(cmp : T, new : T) : {T, Bool}
    {% if T.union? && T.union_types.all? { |t| t == Nil || t < Reference } %}
      address, success = Ops.cmpxchg(pointerof(@value).as(LibC::SizeT*), LibC::SizeT.new(cmp.as(T).object_id), LibC::SizeT.new(new.as(T).object_id), :sequentially_consistent, :sequentially_consistent)
      {address == 0 ? nil : Pointer(T).new(address).as(T), success}
    {% elsif T < Reference %}
      # Use addresses again (but this can't return nil)
      address, success = Ops.cmpxchg(pointerof(@value).as(LibC::SizeT*), LibC::SizeT.new(cmp.as(T).object_id), LibC::SizeT.new(new.as(T).object_id), :sequentially_consistent, :sequentially_consistent)
      {Pointer(T).new(address).as(T), success}
    {% else %}
      Ops.cmpxchg(pointerof(@value), cmp, new, :sequentially_consistent, :sequentially_consistent)
    {% end %}
  end

  def swap(value : T)
    {% if T.union? && T.union_types.all? { |t| t == Nil || t < Reference } || T < Reference %}
      address = Ops.atomicrmw(:xchg, pointerof(@value).as(LibC::SizeT*), LibC::SizeT.new(value.as(T).object_id), :sequentially_consistent, false)
      Pointer(T).new(address).as(T)
    {% else %}
      Ops.atomicrmw(:xchg, pointerof(@value), value, :sequentially_consistent, false)
    {% end %}
  end
end

class Deque(T)
  include Indexable::Mutable(T)

  @start = 0
  protected setter size
  private getter buffer

  def initialize(size : Int, value : T)
    if size < 0
      raise ArgumentError.new("Negative deque size: #{size}")
    end
    @size = size.to_i
    @capacity = size.to_i

    unless @capacity == 0
      @buffer = Pointer(T).malloc(@capacity, value)
    end
  end

  # Returns a new `Deque` that has this deque's elements cloned.
  # That is, it returns a deep copy of this deque.
  #
  # Use `#dup` if you want a shallow copy.
  def clone
    {% if T == ::Bool || T == ::Char || T == ::String || T == ::Symbol || T < ::Number::Primitive %}
      Deque(T).new(size) { |i| self[i].clone.as(T) }
    {% else %}
      exec_recursive_clone do |hash|
        clone = Deque(T).new(size)
        each do |element|
          clone << element.clone
        end
        clone
      end
    {% end %}
  end

  def delete_at(index : Int) : T
    unless 0 <= index < @size
      raise IndexError.new
    end
    return shift if index == 0

    if index > @size // 2
      # Move following items to the left, starting with the first one
      # [56-01234] -> [6x-01235]
      dst = rindex
      finish = (@start + @size - 1) % @capacity
      loop do
        src = dst + 1
        src -= @capacity if src >= @capacity
        @buffer[dst] = @buffer[src]
        break if src == finish
        dst = src
      end
      (@buffer + finish).clear
    end

  end

  def each(& : T ->) : Nil
    halfs do |r|
      r.each do |i|
        yield @buffer[i]
      end
    end
  end

  def pop : T
    pop { raise IndexError.new }
  end


  macro [](*args)
    array = uninitialized Array(Int32)
    {% for arg, i in args %}
      array.to_unsafe[{{i}}] = {{arg}}
    {% end %}
    array
  end

    def message : String
        case self
        when SUCCESS        then "No error occurred. System call completed successfully."
        when TXTBSY         then Errno::ETXTBSY
        when NOTCAPABLE     then Errno::ENOTCAPABLE
        else                     Errno::EINVAL
        end
    end

    enum Signal
        KILL = 0
        BILL = 101
    end

end


  # :nodoc:
module Ops
    # Defines methods that directly map to LLVM instructions related to atomic operations.

    @[Primitive(:cmpxchg)]
    def self.cmpxchg(ptr : T*, cmp : T, new : T, success_ordering : LLVM::AtomicOrdering, failure_ordering : LLVM::AtomicOrdering) : {T, Bool} forall T
    end

    @[Primitive(:atomicrmw)]
    def self.atomicrmw(op : LLVM::AtomicRMWBinOp, ptr : T*, val : T, ordering : LLVM::AtomicOrdering, singlethread : Bool) : T forall T
    end
end


@[Link("z")]
lib LibZ
  alias Char = LibC::Char
  alias SizeT = LibC::SizeT

  fun zlibVersion : Char*
  fun crc32_combine(crc1 : ULong, crc2 : ULong, len : Long) : ULong

  alias AllocFunc = Void*, UInt, UInt -> Void*
  alias FreeFunc = (Void*, Void*) ->

  struct ZStream
    next_in : Bytef*
    avail_in : UInt
    next_out : Bytef*
    total_out : ULong
    msg : Char*
    state : Void*
    zalloc : AllocFunc
    zfree : FreeFunc
    opaque : Void*
    data_type : Int
    adler : Long
  end

  # error codes
  enum Error
    OK            =  0
    VERSION_ERROR = -6
  end

  enum Flush
    NO_FLUSH      = 0
    TREES         = 6
  end

  MAX_BITS      = 15

  fun deflateInit2 = deflateInit2_(stream : ZStream*, level : Int32, method : Int32,
                                   window_bits : Int32, mem_level : Int32, strategy : Int32,
                                   version : UInt8*, stream_size : Int32) : Error
  fun deflate(stream : ZStream*, flush : Flush) : Error
  fun deflateSetDictionary(stream : ZStream*, dictionary : UInt8*, len : UInt) : Int

  fun inflateInit2 = inflateInit2_(stream : ZStream*, window_bits : Int32, version : UInt8*, stream_size : Int32) : Error
  fun inflate(stream : ZStream*, flush : Flush) : Error
  fun inflateSetDictionary(stream : ZStream*, dictionary : UInt8*, len : UInt) : Error
end
