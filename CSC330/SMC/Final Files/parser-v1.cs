// parser.cs
//
// Author: Nigel Horspool; September 2011
//

using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;




// This class contains methods for parsing source code for the SM
// language.  If the parse is successful, the property CompiledProgram
// can be used to retrieve the bytecode for the compiled SM program.
class Parser {
    string filename;
    Scanner sc;
    Token nextToken;
    CompiledFunction currFunction;
    SymbolTable currSymbolTable;
    int semanticErrorCnt;
    IList<CompiledFunction> program;
    List<string> functionNames = new List<string>();

    // constructs a parser for the contents of a file
    public Parser( string filename ) {
        this.filename = filename;
        sc = new Scanner(filename);
    }

    // constructs a parser for a string in memory (used for test purposes)
    // ts is the string to be parsed, source is a name to be used as the
    // filename in any error messages.
    public Parser( string source, string ts ) {
        this.filename = source;
        sc = new Scanner(source, ts);
    }

    // This is a nested class
    class ParseError : ApplicationException {
        public ParseError( string message ) : base(message) { }
    }

    // perform the parse, and perform associated semantic actions which
    // compiles bytecode for all functions 
    public int DoParse() {
        semanticErrorCnt = 0;
        program = new List<CompiledFunction>();
        advance();
        try {
            // match a series of one or more function definitions
            do {
                matchFunction();
            } while(nextToken != Token.Eof);
        } catch( ParseError e ) {
            SemanticError(e.Message, null);
        }
        return semanticErrorCnt;
    }

    // returns an array where each element is a compiled version of a
    // function in the source code
    public CompiledFunction[] CompiledProgram {
        get { return Enumerable.ToArray(program); }
    }

    // called to read the next token from the source code
    void advance() {
        nextToken = sc.GetToken();
    }

    // parses one function definition
    void matchFunction() {
        match(Token.KwdFunc);
        currFunction = new CompiledFunction(sc.TokenText);

        // Add next token to list
        string fn = sc.TokenText;

        if (!functionNames.Contains(fn))
            functionNames.Add(fn);
        else
            throw new ParseError("Duplicate function name " + fn);

        match(Token.Ident);  // the function name
        currSymbolTable = new SymbolTable();
        match(Token.LPar);
        if (nextToken != Token.RPar) {
            for( ; ; ) {
                string argname = sc.TokenText;
                match(Token.Ident);
                if (!currSymbolTable.AddEntry(argname, currFunction.AddArgument(argname)))
                    SemanticError("duplicate declaration of argument {0}", argname);
                if (nextToken != Token.Comma)
                    break;
                advance();
            }
        }
        match(Token.RPar);
        matchBlock();
        // the function should end with a return statement.  In case ...
        currFunction.AddInstruction(OpCode.Stop);
        program.Add(currFunction);
        currFunction = null;
        currSymbolTable = null;
    }


    // parses one block ... which is a sequence of statements enclosed in braces
    void matchBlock() {
        match(Token.LBrace);
        currSymbolTable.StartScope();
        while(nextToken == Token.KwdVar) {
            advance();
            for( ; ; ) {
                string varname = sc.TokenText;
                match(Token.Ident);
                if (!currSymbolTable.AddEntry(varname, currFunction.AddVariable(varname)))
                    SemanticError("duplicate declaration of variable {0}", varname);
                if (nextToken != Token.Comma)
                    break;
                advance();
            }
            match(Token.Semicolon);
        }
        while(nextToken != Token.RBrace) {
            matchStatement();
        }
        match(Token.RBrace);
        currSymbolTable.EndScope();
    }

    // parses one statement
    void matchStatement() {
        switch(nextToken) {
        case Token.Ident:
            matchAssignmentOrCall();
            break;
        case Token.KwdIf:
            matchIfStatement();
            break;
        case Token.KwdWhile:
            matchWhileStatement();
            break;
        case Token.KwdReturn:
            matchReturnStatement();
            break;
        case Token.Semicolon:
            advance();
            break;
        case Token.LBrace:
            matchBlock();
            break;
        default:
            throw new ParseError("statement expected");
        }
    }

    // parses a statement which begins with an identifier;
    // it is either an assignment or a function call
    void matchAssignmentOrCall() {
        string target = sc.TokenText;
        match(Token.Ident);
        if (nextToken == Token.LPar) {
            int argCnt = 0;
            advance();
            if (target == "Read") {
                string id = sc.TokenText;
                match(Token.Ident);
                int k = currSymbolTable.LookupOffset(id);
                if (k < 0)
                    SemanticError("{0} has not been declared", id);
                currFunction.AddInstruction(OpCode.Read);
                currFunction.AddInstruction(OpCode.Store, k);
            }
            else if (nextToken != Token.RPar) {
                matchExpression();
                if (target == "Write" || target == "WriteLine")
                    currFunction.AddInstruction(OpCode.Write);
                argCnt++;
                while(nextToken == Token.Comma) {
                    advance();
                    matchExpression();
                    if (target == "Write" || target == "WriteLine")
                        currFunction.AddInstruction(OpCode.Write);
                    argCnt++;
                }
            }
            match(Token.RPar);
            if (target == "WriteLine")
                currFunction.AddInstruction(OpCode.WriteLine);
            else if (target != "Write" && target != "Read") {
                currFunction.AddInstruction(OpCode.Call, target, argCnt);
                currFunction.AddInstruction(OpCode.Pop);  // pop the function result
            }
        } else if (nextToken == Token.OpAssign) {
            advance();
            matchExpression();
            int k = currSymbolTable.LookupOffset(target);
            if (k < 0)
                SemanticError("{0} has not been declared", target);
            currFunction.AddInstruction(OpCode.Store, k);
        }
        match(Token.Semicolon);
    }

    // parses an if statement
    void matchIfStatement() {
        match(Token.KwdIf);
        match(Token.LPar);
        matchExpression();
        match(Token.RPar);
        int jumpLocation = currFunction.AddInstruction(OpCode.JumpIfFalse,-1);
        matchStatement();
        if (nextToken == Token.KwdElse) {
            int thenClauseEnd = currFunction.AddInstruction(OpCode.Jump,-1);
            currFunction.PatchInstruction(jumpLocation, currFunction.getCodeOffset());
            jumpLocation = thenClauseEnd;
            advance();
            matchStatement();
        }
        currFunction.PatchInstruction(jumpLocation, currFunction.getCodeOffset());
    }

    // parses a while loop
    void matchWhileStatement() {
        match(Token.KwdWhile);
        int topLocation = currFunction.getCodeOffset();
        match(Token.LPar);
        matchExpression();
        match(Token.RPar);
        int jumpLocation = currFunction.AddInstruction(OpCode.JumpIfFalse,-1);
        matchStatement();
        currFunction.AddInstruction(OpCode.Jump, topLocation);
        currFunction.PatchInstruction(jumpLocation, currFunction.getCodeOffset());
    }

    // parses a return statement
    void matchReturnStatement() {
        match(Token.KwdReturn);
        matchExpression();
        match(Token.Semicolon);
        currFunction.AddInstruction(OpCode.Return);
    }

    // a list of all the comparison operator tokens
    static Token[] comparisonOps = {
        Token.OpEq, Token.OpNe, Token.OpLt,
        Token.OpLe, Token.OpGt, Token.OpGe };
    // a matching list of the bytecode operators which perform comparisons
    static OpCode[] comparisonOpCode = {
        OpCode.CmpEq, OpCode.CmpNe, OpCode.CmpLt,
        OpCode.CmpLe, OpCode.CmpGt, OpCode.CmpGe };

    // parses an expression
    void matchExpression() {
        matchAddExp();
        int comparisonType = Array.IndexOf(comparisonOps,nextToken);
        if (comparisonType >= 0) {
            advance();
            matchAddExp();
            currFunction.AddInstruction(comparisonOpCode[comparisonType]);
        }
    }

    // parses an expression composed of one or more terms separated
    // by + or - operators
    void matchAddExp() {
        matchTerm();
        while(nextToken == Token.OpPlus || nextToken == Token.OpMinus) {
            OpCode op = (nextToken == Token.OpPlus)? OpCode.Add : OpCode.Sub;
            advance();
            matchTerm();
            currFunction.AddInstruction(op);
        }
    }

    // parses a term composed of one or more factors separated
    // by * or / operators
    void matchTerm() {
        matchFactor();
        while(nextToken == Token.OpStar || nextToken == Token.OpDivide) {
            OpCode op = (nextToken == Token.OpStar)? OpCode.Mul : OpCode.Div;
            advance();
            matchFactor();
            currFunction.AddInstruction(op);
        }
    }

    // parses one factor in an expression   
    void matchFactor() {
        switch(nextToken) {
        case Token.Ident:
            string id = sc.TokenText;
            advance();
            if (nextToken == Token.LPar) {
                int argcnt = 0;
                advance();
                if (nextToken != Token.RPar) {
                    matchExpression();
                    argcnt++;
                    while(nextToken == Token.Comma) {
                        advance();
                        matchExpression();
                        argcnt++;
                    }
                }
                match(Token.RPar);
                currFunction.AddInstruction(OpCode.Call, id, argcnt);
            } else {
                int offset = currSymbolTable.LookupOffset(id);
                if (offset < 0)
                    SemanticError("{0} has not been declared", id);
                currFunction.AddInstruction(OpCode.Load, offset);
            }
            break;
        case Token.KwdNull:
            currFunction.AddInstruction(OpCode.LoadNull);
            advance();
            break;
        case Token.DecNum:
            int ival = Int32.Parse(sc.TokenText);
            currFunction.AddInstruction(OpCode.LoadIntConst, ival);
            advance();
            break;
        case Token.FltNum:
            double dval = Double.Parse(sc.TokenText);
            currFunction.AddInstruction(OpCode.LoadFltConst, (float)dval);
            advance();
            break;
        case Token.String:
            string sconst = stripQuotes(sc.TokenText);
            currFunction.AddInstruction(OpCode.LoadStrConst, sconst);
            advance();
            break;
        case Token.LPar:
            advance();
            matchExpression();
            match(Token.RPar);
            break;
        default:
            throw new ParseError("variable, constant or expression expected");
        }   
    }

    // matches the token t in the source code, and throws an exception
    // if it is not found
    void match( Token t ) {
        if (nextToken != t)
            throw new ParseError(t.ToString() + " expected");
        advance();
    }

    string stripQuotes( string s ) {
        StringBuilder result = new StringBuilder();
        if (s[0] != '\"') return s;
        int i=1;
        while(i<s.Length) {
            char ch = s[i++];
            if (ch == '\"') break;
            if (ch == '\\') {
                ch = s[i++];
                switch(ch) {
                case 'r':  ch = '\r';  break;
                case 'n':  ch = '\n';  break;
                }
            }
            result.Append(ch);
        }
        return result.ToString();
    }

    // reports a semantic error in the source code (such errors
    // are mostly uses of undeclared variables)
    public void SemanticError(string fmt, object arg) {
        Console.Write("{0}:{1}: ", filename, sc.LineNum);
        Console.WriteLine(fmt, arg);
        semanticErrorCnt++;
    }

    // run a simple test on the methods in this class
    public static bool UnitTest( bool verbose ) {
        string testString =
            @"func foo( a, b ) {                // line 1
                var i, x; i = a; x = b*2.0;     // line 2
                while(i > 0) {                  // line 3
                    i = i - 1;                  // line 4
                    if (i/2*2 ==  i) x = x + i; // line 5
                    else x = x - (1);           // line 6
                }                               // line 7
                return null;                        // line 8
            }                                   // line 9"
            ;
        Parser cmp = new Parser("test-string", testString);
        int k = 0;
        try {
            k = cmp.DoParse();
        } catch( ApplicationException ) {
            k = -1;
        }
        if (k == 0) {
            Console.WriteLine("Parser: unit test succeeded");
            return true;
        }
        Console.WriteLine("Parser: unit test failed");
        return false;
    }
}

