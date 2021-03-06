// File: interpreter.cs
//
// Author: Nigel Horspool; September 2011
//         Jordan Ell; September 2011
//

using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

public enum TraceOptions { None=0, TraceOps=1, TraceCalls=2, TraceStack = 4 };


public class Interpreter {

    // A nested type.
    // Each element of the SM interpreter's run-time stack has a tag named t
    // with TypeCode type that identifies the run-time datatype of the element.
    public enum TypeCode { None, IntType, FloatType, StringType };

    // A nested declaration of the SMValue datatype
    // This type is used for the elements of the SM interpreter's run-time stack.
    // Each element can have only one SM type at a time ... thus only one of the
    // three fields (intValue, fltValue, strValue) is used.
    public class SMValue
    {
        public TypeCode t;
        public int intValue;
        public float fltValue;
        public String strValue;
        public override string ToString()
        {
            return "???";
        }
    }

    public class SMValueInt : SMValue
    {
        public SMValueInt(int i)
        {
            t = TypeCode.IntType;
            intValue = i;
        }

        public override string ToString()
        {
            return String.Format("{0}", intValue);
        }

    }

    public class SMValueFlt : SMValue
    {
        public SMValueFlt(float f)
        {
            t = TypeCode.FloatType;
            fltValue = f;
        }

        public override string ToString()
        {
            return String.Format("{0}", fltValue);
        }
    }

    public class SMValueStr : SMValue
    {
        public SMValueStr(String s)
        {
            t = TypeCode.StringType;
            strValue = s;
        }

        public override string ToString()
        {
            return strValue == null ? "--null--" : strValue;
        }
    }

    public class SMValueNone : SMValue
    {
        public SMValueNone()
        {
            t = TypeCode.None;
        }
    }


    const int stackSizeInitial = 100;
    const int stackSizeIncrement = 100;
    Dictionary<string,CompiledFunction> function;
    SMValue[] stack;
    int stackHeight = 0;
    TraceOptions traceFlags = 0;

    public Interpreter( CompiledFunction[] bytecode ) {
        function = new Dictionary<string,CompiledFunction>();

        foreach( CompiledFunction cf in bytecode ) {
            string name = cf.Name;
            if (function.ContainsKey(name)) {
                Console.WriteLine("Duplicate declaration of function {0}", name);
                continue;
            }
            function[name] = cf;
        }
    }

    // a nested class for signalling errors found while interpreting
    class RunTimeError: ApplicationException {
        public RunTimeError( string message ) : base(message) { }
    }

    public bool Run( TraceOptions traceFlags ) {
        CompiledFunction ep;
        if (!function.TryGetValue("Main", out ep)) {
            Console.WriteLine("No Main function found");
            return false;
        }
        if (ep.NumArguments != 0) {
            Console.WriteLine("The Main function takes 0 arguments");
            return false;
        }
        this.traceFlags = traceFlags;
        stack = new SMValue[stackSizeInitial];
        stackHeight = 0;
        bool resultCode = executeFunction( ep );
        return resultCode;
    }

    void push( SMValue v ) {
        if (stackHeight >= stack.Length) {
            // grow the stack
            SMValue[] newStack = new SMValue[stackHeight+stackSizeIncrement];
            stack.CopyTo(newStack,0);
            stack = newStack;
        }
        if ((traceFlags & TraceOptions.TraceStack) != TraceOptions.None)
            Console.WriteLine("*     push {0}", v);
        stack[stackHeight++] = v;
    }

    void push( int i ) {
        push( new SMValueInt(i) );
    }

    void push( float f ) {
        push( new SMValueFlt(f) );
    }

    SMValue pop() {
        if (stackHeight <= 0)
            throw new RunTimeError("stack underflow");
        SMValue result = stack[--stackHeight];
        stack[stackHeight] = new SMValue();
        if ((traceFlags & TraceOptions.TraceStack) != TraceOptions.None)
            Console.WriteLine("*     {0} popped", result);
        return result;
    }

    // execute the bytecode of function  fn,
    // using the arguments on the stack and leaving a result on the stack.
    // A result of true indicates that execution completed normally.
    // A result of false indicates that a STOP instruction was executed
    // and interpretation of the whole program should cease.
    bool executeFunction( CompiledFunction fn ) {
        // allocate slots for local variables on the stack
        int firstLocal = stackHeight - fn.NumArguments;
        for( int i = fn.NumArguments; i < fn.NumLocals; i++ )
            push(new SMValue());

        int pc = 0;

        try {
            int    iopnd;
            float  fopnd;
            string sopnd;
            SMValue val;
            for( ; ; ) {
                Instruction ins = fn.GetInstructionAt(pc++);
                if ((traceFlags & TraceOptions.TraceOps) != TraceOptions.None)
                    Console.WriteLine("* {0}: {1}", pc-1, ins);
                switch(ins.Op) {
                case OpCode.Load:
                    iopnd = ((InstructionInt)ins).IntOperand;
                    if (iopnd >= fn.NumLocals)
                        throw new RunTimeError("invalid immediate operand");
                    val = stack[iopnd+firstLocal];
                    if (val.t == TypeCode.None)
                        throw new RunTimeError("uninitialized variable used");
                    push(val);
                    break;
                case OpCode.Store:
                    iopnd = ((InstructionInt)ins).IntOperand;
                    if (iopnd >= fn.NumLocals)
                        throw new RunTimeError("invalid immediate operand");
                    stack[iopnd+firstLocal] = pop();
                    break;
                case OpCode.LoadIntConst:
                    iopnd = ((InstructionInt)ins).IntOperand;
                    push(new SMValueInt(iopnd));
                    break;
                case OpCode.LoadFltConst:
                    fopnd = ((InstructionFlt)ins).FltOperand;
                    push(new SMValueFlt(fopnd));
                    break;
                case OpCode.LoadStrConst:
                    sopnd = ((InstructionStr)ins).StrOperand;
                    push(new SMValueStr(sopnd));
                    break;
                case OpCode.LoadNull:
                    push(new SMValue());
                    break;
                case OpCode.Jump:
                    pc = ((InstructionInt)ins).IntOperand;
                    break;
                case OpCode.JumpIfFalse:
                    val = pop();
                    if (val.t != TypeCode.IntType)
                        throw new RunTimeError("type mismatch, int value required");
                    if (val.intValue == 0)
                        pc = ((InstructionInt)ins).IntOperand;
                    break;
                case OpCode.Call:
                    sopnd = ((InstructionStrInt)ins).Name;
                    iopnd = ((InstructionStrInt)ins).NumArgs;
                    CompiledFunction callee = null;
                    if (!function.TryGetValue(sopnd, out callee))
                        throw new RunTimeError(
                            String.Format("call to undefined function {0}", sopnd));
                    if (callee.NumArguments != iopnd)
                        throw new RunTimeError(
                            String.Format("call with incorrect number of arguments to function {0}", sopnd));
                    if (!executeFunction(callee))
                        return false;  // failure return
                    break;
                case OpCode.Pop:
                    pop();
                    break;
                case OpCode.Return:
                    val = pop(); // this is the function result
                    // now pop all the local variables (including the arguments)
                    for( int j = fn.NumLocals; j > 0; j-- )
                        pop();
                    push(val);  // put the function result back
                    return true;  // success return
                case OpCode.Add:
                case OpCode.Sub:
                case OpCode.Mul:
                case OpCode.Div:
                case OpCode.CmpEq:
                case OpCode.CmpNe:
                case OpCode.CmpGt:
                case OpCode.CmpGe:
                case OpCode.CmpLt:
                case OpCode.CmpLe:
                    SMValue val2 = pop();  // 2nd operand is above the 1st on the stack!
                    SMValue val1 = pop();
                    if (val1.t == TypeCode.IntType && val2.t == TypeCode.IntType)
                        doIntOp(ins.Op, val1.intValue, val2.intValue);
                    else {
                        float flt1 = (val1.t == TypeCode.FloatType)? val1.fltValue : val1.intValue;
                        float flt2 = (val2.t == TypeCode.FloatType)? val2.fltValue : val2.intValue;
                        doFltOp(ins.Op, flt1, flt2);
                    }
                    break;
                case OpCode.Stop:
                    Console.WriteLine("Program halted (stop instruction)");
                    return false;
                case OpCode.Read:
                    push(readValue());
                    break;
                case OpCode.Write:
                    val = pop();
                    Console.Write(val.ToString());
                    break;
                case OpCode.WriteLine:
                    Console.WriteLine();
                    break;
                default:
                    throw new RunTimeError("unimplemented opcode");
                }
            }
        } catch( ApplicationException e ) {
            Console.WriteLine("Execution aborted due to run-time error:\n\t{0}", e.Message);
        }
        return false;
    }
    
    void doIntOp( OpCode op, int opnd1, int opnd2 ) {
        switch(op) {
        case OpCode.Add:    push(opnd1 + opnd2);  break;
        case OpCode.Sub:    push(opnd1 - opnd2);  break;
        case OpCode.Mul:    push(opnd1 * opnd2);  break;
        case OpCode.Div:    push(opnd1 / opnd2);  break;
        case OpCode.CmpEq:  push(opnd1 == opnd2? 1 : 0) ;  break;
        case OpCode.CmpNe:  push(opnd1 != opnd2? 1 : 0) ;  break;
        case OpCode.CmpGt:  push(opnd1 > opnd2? 1 : 0) ;   break;
        case OpCode.CmpGe:  push(opnd1 >= opnd2? 1 : 0) ;  break;
        case OpCode.CmpLt:  push(opnd1 < opnd2? 1 : 0) ;   break;
        case OpCode.CmpLe:  push(opnd1 <= opnd2? 1 : 0) ;  break;
        default:
            throw new RunTimeError("should not happen!");
        }   
    }
    
    void doFltOp( OpCode op, float opnd1, float opnd2 ) {
        switch(op) {
        case OpCode.Add:    push(opnd1 + opnd2);  break;
        case OpCode.Sub:    push(opnd1 - opnd2);  break;
        case OpCode.Mul:    push(opnd1 * opnd2);  break;
        case OpCode.Div:    push(opnd1 / opnd2);  break;
        case OpCode.CmpEq:  push(opnd1 == opnd2? 1 : 0) ;  break;
        case OpCode.CmpNe:  push(opnd1 != opnd2? 1 : 0) ;  break;
        case OpCode.CmpGt:  push(opnd1 > opnd2? 1 : 0) ;   break;
        case OpCode.CmpGe:  push(opnd1 >= opnd2? 1 : 0) ;  break;
        case OpCode.CmpLt:  push(opnd1 < opnd2? 1 : 0) ;   break;
        case OpCode.CmpLe:  push(opnd1 <= opnd2? 1 : 0) ;  break;
        default:
            throw new RunTimeError("should not happen!");
        }   
    }
    
    // Read characters from standard input to form a value
    SMValue readValue() {
        Regex fltpat = new Regex( @"^\s*([\+\-]?\d+\.\d*(?:[eE][\+\-]?\d+)?)" );
        Regex intpat = new Regex( @"^\s*([\+\-]?\d+)" );
        Regex strpat = new Regex( "^\\s*\"([^\"]*)\"" );
        string line = Console.ReadLine();
        Match m;
        m = fltpat.Match(line);
        if (m.Success) {
            double d = Double.Parse(m.Groups[1].ToString());
            return new SMValueFlt((float)d);
        }
        m = intpat.Match(line);
        if (m.Success) {
            int i = Int32.Parse(m.Groups[1].ToString());
            return new SMValueInt(i);
        }
        m = strpat.Match(line);
        if (m.Success) {
            string s = m.Groups[1].ToString();
            return new SMValueStr(s);
        }
        if (!Regex.Match(line, "^\\w*$").Success)
            throw new RunTimeError("bad input line: "+line);
        return readValue();  // it was an empty line, try again
    }
}