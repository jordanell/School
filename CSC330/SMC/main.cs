// File main.cs
//
// Author: Nigel Horspool;  February 2011
//

using System;
using System.Diagnostics; // needed for the Process class


public class SMCompileAndInterpret {
    static int testOption = 0;
    static TraceOptions traceOption = TraceOptions.None;
    static bool dumpCodeOption = false;

    public static void Usage() {
        Console.WriteLine("Usage:");
        Console.WriteLine("\t1.\tcommand  -test");
        Console.WriteLine("\t2.\tcommand  [options] filename");
        Console.WriteLine("where possible options are:");
        Console.WriteLine("\t-dump\tdump the bytecode after compiling");
        Console.WriteLine("\t-traceOps\ttrace individual bytecode op execution");
        Console.WriteLine("\t-traceCalls\ttrace bytecode function call/return");
        Console.WriteLine("\t-traceStack\ttrace stack values pushed/popped");
        Console.WriteLine("\t-traceAll\ttrace all of above");
        Environment.Exit(1);
    }

    static void enableOption( string s ) {
        switch(s) {
        case "-test":
            testOption = 1;
            break;
        case "-Test":
            testOption = 2;
            break;
        case "-dump":
            dumpCodeOption = true;
            break;
        case "-traceOps":
            traceOption |= TraceOptions.TraceOps;
            break;
        case "-traceCalls":
            traceOption |= TraceOptions.TraceCalls;
            break;
        case "-traceStack":
            traceOption |= TraceOptions.TraceStack;
            break;
        case "-traceAll":
            traceOption = (TraceOptions)255;
            break;
        default:
            Usage();
            break;
        }
    }

    static void Main( string[] args ) {
        // There must be at least one command line argument
        if (args == null || args.Length < 1) {
            Usage();
        }
        string sourceFile = null;
        foreach( string s in args ) {
            if (s.StartsWith("-")) {
                enableOption(s);
                continue;
            }
            if (sourceFile != null) {
                Usage();
            }
            sourceFile = s;
        }

        // A special mode for just running unit tests on the classes
        if (testOption > 0) {
            Scanner.UnitTest(testOption > 1);
            Parser.UnitTest(testOption > 1);
            CompiledFunction.UnitTest(testOption > 1);
            return;
        }

        if (sourceFile == null)
            Usage();

        if (!sourceFile.EndsWith(".sm")) {
            Console.WriteLine("Source file name must have '.sm' suffix");
            return;
        }

        try {
            // scan and parse the source files
            Parser cmp = new Parser(sourceFile);
            int errorCnt = cmp.DoParse();
            CompiledFunction[] bytecode = cmp.CompiledProgram;

            if (dumpCodeOption) {
                // dump bytecode for each function
                foreach( CompiledFunction f in bytecode ) {
                    f.DumpBytecode();
                }
            }
            
            if (errorCnt == 0) {
                // run the SM program, and track how many CPU seconds it takes
                Interpreter intp = new Interpreter(bytecode);
                Process thisProcess = Process.GetCurrentProcess();
                TimeSpan startTime = thisProcess.UserProcessorTime;

                intp.Run(traceOption);

                TimeSpan endTime = thisProcess.UserProcessorTime;
                TimeSpan executionTime = endTime.Subtract(startTime);
                Console.WriteLine("\n** Execution time = {0} (hours:minutes:seconds)", executionTime);
            }

        } catch( ApplicationException ) {
            Console.WriteLine("{0}: translation terminated due to errors", sourceFile);
        }
    }
}
