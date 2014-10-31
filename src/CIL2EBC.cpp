
/*
		名称: CIL2EBC 源对源翻译
		作者: 南宁廖华<nnliaohua@yeah.net> 版权所有
		功能: 将 VS2013 输出的 CIL .asm 源程序翻译为 EBC .EBCasm 源程序
		版本: 0.1
*/

/*
		name: CIL2EBC source to source translation
		author: nnliaohua<nnliaohua@yeah.net> Copyright
		purpose: translate VS2013's output CIL .asm source to EBC .EBCasm source.
		version: 0.1
*/

typedef unsigned char			__u8;
typedef signed char				__i8;
typedef unsigned short int		__u16;
typedef signed short int		__i16;
typedef unsigned long int		__u32;
typedef signed long int			__i32;
typedef unsigned long long int	__u64;
typedef signed long long int	__i64;

#include <stdio.h>


#define STATUS_DONE		0
#define STATUS_ERR		1

static __i16 init_parser(char * infn);
static void terminate_parser(void);
static __i16 start_parser(void);

int main(int argc, char *argv[])
{
	int status;

	if (argc != 2)
	{
		fprintf(stderr, "用法: CIL2EBC 文件名.asm\nUsage: CIL2EBC filename.asm\n");
		return STATUS_ERR;
	}
	status = init_parser(argv[1]);
	if (status == STATUS_ERR)
	{ return STATUS_ERR; }
	status = start_parser();
	if (status == STATUS_ERR)
	{ 
		terminate_parser();
		return STATUS_ERR; 
	}
	return STATUS_DONE;
}


/*
	词法分析器
	1. 文件读取函数集
	2. 词法分析器
*/

/*
	lexics
	1. file read package
	2. lexics
*/

/*
	1. 文件读取函数集
	1. file read package
*/

#define BUFF_SIZE	64*1024

static __i16	inbuff[BUFF_SIZE];
static __i32	content_len;
static __i32	buff_pos;

static char		*infname;
static FILE		*fp;


static void init_lex(void);

static __i16 init_parser(char * infn)
{
	__i32 readlen;

	infname = infn;
	fp = fopen(infname, "rb");
	if (!fp)
	{
		fprintf(stderr, "Error: can not open file [%s]\n", infname);
		return STATUS_ERR;
	}
	readlen = fread(inbuff, sizeof(__u16), BUFF_SIZE, fp);
	if (!readlen)
	{
		fprintf(stderr, "Error: empty file or read error!\n");
		fclose(fp);
		return STATUS_ERR;
	}
	if (inbuff[0] != (__i16)0xFEFF)
	{
		fprintf(stderr, "Error: file must be Unicode litte endian format!\n");
		fclose(fp);
		return STATUS_ERR;
	}
	content_len = readlen;
	buff_pos = 1;
	init_lex();
	return STATUS_DONE;
}

static void terminate_parser(void)
{
	fclose(fp);
}

const __i16 CHAR_EOF = -1;

__i16 getnextchar(void)
{
	__i32 readlen;
	__i16 curchar;

	if ( buff_pos == content_len )
	{
		readlen = fread(inbuff, sizeof(__u16), BUFF_SIZE, fp);
		if (!readlen)
		{
			fclose(fp);
			return CHAR_EOF;
		}
		content_len = readlen;
		buff_pos = 0;
	}
	curchar = inbuff[buff_pos];
	buff_pos++;
	return curchar;
}

/*
	2. 词法分析器
	2. lexics
*/

// 关键字-语法动作对
// keyword-action pair

typedef struct st_keyword_action_pair
{
	__i8	keyword[24];
	__i16	action;
} sKApair;

// 语法动作定义
// syntax action definition

#define BEGIN_PARSE		10
#define END_PARSE		11
#define ILL_KW			12
#define NEED_S1			13
#define NEED_LABEL		14
#define NEED_0			15
#define NEED_1			16
#define NEED_2			17
#define NEED_3			18
#define NEED_4			19
#define NEED_5			20
#define NEED_6			21
#define NEED_7			22
#define NEED_8			23
#define NEED_QSTRING	24
#define NEED_L_C_N		25
#define NEED_COMMA_N	26
#define NEED_NUMBER		27
#define NEED_NOTHING	28
#define NEED_COLON		29
#define COMPLEX_1		30
#define NEED_TEXT_LABEL	31
#define NEED_A_C_L		32
#define NEED_SNUMBER	33

/* CIL 伪指令定义, 根据的是 [ECMA-335] [附录 C], 它比正文里面多了很多东西. */
/* CIL directives definition, according to [ECMA-335] [Annex C], it have more thing than official contents. */

// #line 没见过, 直接略去了
// #line never meet, ignore directly.

#define MAX_DIRECTIVES		61
#define DIRECTIVE_OTHER		MAX_DIRECTIVES + 1

#define INDEX_TEXT			53

static const sKApair directives[MAX_DIRECTIVES] =
{
	{ ".addon",			ILL_KW },
	// 又是两个没有的, 微软方言
	// two not have again, microsoft dialect.
	{ ".align",			NEED_NUMBER },
	{ ".ascii",			COMPLEX_1 },
	{ ".assembly",		ILL_KW },
	// 又是一个没有的, 微软方言
	// a don't have again, microsoft dialect.
	{ ".bss",			NEED_NOTHING },
	{ ".cctor",			ILL_KW },
	{ ".class",			ILL_KW },
	// 又是一个没有的, 微软方言
	// a don't have again, microsoft dialect.
	{ ".comdat",		NEED_A_C_L },
	{ ".corflags",		ILL_KW },
	{ ".ctor",			ILL_KW },
	{ ".custom",		ILL_KW },
	{ ".data",			ILL_KW },
	{ ".emitbyte",		ILL_KW },
	// 又是一个没有的, 微软方言
	// a don't have again, microsoft dialect.
	{ ".end",			NEED_LABEL },
	{ ".entrypoint",	ILL_KW },
	{ ".event",			ILL_KW },
	{ ".export",		ILL_KW },
	{ ".field",			ILL_KW },
	{ ".file",			NEED_QSTRING },
	{ ".fire",			ILL_KW },
	{ ".get",			ILL_KW },
	// 见鬼啊! 这个 ECMA-335 里面怎么连 .global 都没有的啊!
	// GOD damn! why this ECMA-335 have no .global!
	{ ".global",		NEED_LABEL },
	{ ".hash",			ILL_KW },
	{ ".imagebase",		ILL_KW },
	{ ".import",		ILL_KW },
	{ ".language",		ILL_KW },
	{ ".line",			ILL_KW },
	// 又是一个没有的, 微软方言
	// a don't have again, microsoft dialect.
	{ ".local",			NEED_L_C_N },
	{ ".locale",		ILL_KW },
	{ ".localized",		ILL_KW },
	{ ".locals",		ILL_KW },
	{ ".manifestres",	ILL_KW },
	{ ".maxstack",		ILL_KW },
	{ ".method",		ILL_KW },
	{ ".module",		ILL_KW },
	{ ".mresource",		ILL_KW },
	{ ".namespace",		ILL_KW },
	{ ".other",			ILL_KW },
	{ ".override",		ILL_KW },
	{ ".pack",			ILL_KW },
	{ ".param",			ILL_KW },
	{ ".pdirect",		ILL_KW },
	{ ".permission",	ILL_KW },
	{ ".permissionset",	ILL_KW },
	{ ".property",		ILL_KW },
	{ ".publickey",		ILL_KW },
	{ ".publickeytoken",	ILL_KW },
	// 这个是已知为没有的
	// this is known don't have.
	{ ".rdata",			NEED_NOTHING },
	{ ".removeon",		ILL_KW },
	{ ".set",			ILL_KW },
	{ ".size",			ILL_KW },
	// 又是一个没有的, 微软方言
	// a don't have again, microsoft dialect.
	{ ".space",			NEED_NUMBER },
	{ ".subsystem",		ILL_KW },
	// 又是两个没有的, 微软方言
	// two don't have again, microsoft dialect.
	{ ".text",			NEED_NOTHING },
	{ ".textseg",		NEED_TEXT_LABEL },
	{ ".try",			ILL_KW },
	{ ".ver",			ILL_KW },
	{ ".vtable",		ILL_KW },
	{ ".vtentry",		ILL_KW },
	{ ".vtfixup",		ILL_KW },
	{ ".zeroinit",		ILL_KW },
};

/* 在 [ECMA-335] 的 [附录 C] 里面, 有那么一个奇怪的东西, 不知道是什么, 反正我照着实现 */
/* At [ECMA-335] [Annex C], there is a strange thing, I don't know what it is, just simply implement it. */

static const __i8 THE_END[] = "^THE_END^";

/* 普通关键字定义, 根据的也是 [ECMA-335] [附录 C], 它的内容比正文里面描述的内容多. */
/* normal keyword definition, according to [ECMA-335] [Annex C] too, its contents is more than official document. */

#define	MAX_KEYWORDS	425
#define KEYWORD_OTHER	MAX_KEYWORDS + 1
#define KEYWORD_OTHER_1	MAX_KEYWORDS + 2
#define KEYWORD_OTHER_2	MAX_KEYWORDS + 3

#define INDEX_ANY			8
#define INDEX_NODUPLICATE	286

static const sKApair keywords[MAX_KEYWORDS] =
{
	{ "abstract",		ILL_KW },
	{ "add",			NEED_NOTHING },
	{ "add.ovf",		ILL_KW },
	{ "add.ovf.un",		ILL_KW },
	{ "algorithm",		ILL_KW },
	{ "alignment",		ILL_KW },
	{ "and",			NEED_NOTHING },
	{ "ansi",			ILL_KW },
	{ "any",			ILL_KW },
	{ "arglist",		ILL_KW },
	{ "array",			ILL_KW },
	{ "as",				ILL_KW },
	{ "assembly",		ILL_KW },
	{ "assert",			ILL_KW },
	{ "at",				ILL_KW },
	{ "auto",			ILL_KW },
	{ "autochar",		ILL_KW },	
	{ "beforefieldinit",	ILL_KW },
	{ "beq",			NEED_LABEL },
	{ "beq.s",			NEED_LABEL },
	{ "bge",			NEED_LABEL },
	{ "bge.s",			NEED_LABEL },
	{ "bge.un",			NEED_LABEL },
	{ "bge.un.s",		NEED_LABEL },
	{ "bgt",			NEED_LABEL },
	{ "bgt.s",			NEED_LABEL },
	{ "bgt.un",			NEED_LABEL },
	{ "bgt.un.s",		NEED_LABEL },
	{ "ble",			NEED_LABEL },
	{ "ble.s",			NEED_LABEL },
	{ "ble.un",			NEED_LABEL },
	{ "ble.un.s",		NEED_LABEL },
	{ "blob",			ILL_KW },
	{ "blob_object",	ILL_KW },
	{ "blt",			NEED_LABEL },
	{ "blt.s",			NEED_LABEL },
	{ "blt.un",			NEED_LABEL },
	{ "blt.un.s",		NEED_LABEL },
	{ "bne.un",			NEED_LABEL },
	{ "bne.un.s",		NEED_LABEL },
	{ "bool",			ILL_KW },
	{ "box",			ILL_KW },
	{ "br",				NEED_LABEL },
	{ "br.s",			NEED_LABEL },
	{ "break",			ILL_KW },
	{ "brfalse",		NEED_LABEL },
	{ "brfalse.s",		NEED_LABEL },
	{ "brinst",			ILL_KW },
	{ "brinst.s",		ILL_KW },
	{ "brnull",			ILL_KW },
	{ "brnull.s",		ILL_KW },
	{ "brtrue",			NEED_LABEL },
	{ "brtrue.s",		NEED_LABEL },
	{ "brzero",			ILL_KW },
	{ "brzero.s",		ILL_KW },
	{ "bstr",			ILL_KW },
	{ "bytearray",		ILL_KW },
	{ "byvalstr",		ILL_KW },
	{ "call",			NEED_LABEL },
	{ "calli",			ILL_KW },
	{ "callmostderived",	ILL_KW },
	{ "callvirt",		ILL_KW },
	{ "carray",			ILL_KW },
	{ "castclass",		ILL_KW },
	{ "catch",			ILL_KW },
	{ "cdecl",			ILL_KW },
	{ "ceq",			ILL_KW },
	{ "cf",				ILL_KW },
	{ "cgt",			ILL_KW },
	{ "cgt.un",			ILL_KW },
	{ "char",			ILL_KW },
	{ "cil",			ILL_KW },
	{ "ckfinite",		ILL_KW },
	{ "class",			ILL_KW },
	{ "clsid",			ILL_KW },
	{ "clt",			ILL_KW },
	{ "clt.un",			ILL_KW },
	{ "const",			ILL_KW },
	{ "constrained.",	ILL_KW },
	{ "conv.i",			ILL_KW },
	{ "conv.i1",		NEED_NOTHING },
	{ "conv.i2",		NEED_NOTHING },
	{ "conv.i4",		NEED_NOTHING },
	{ "conv.i8",		NEED_NOTHING },
	{ "conv.ovf.i",		ILL_KW },
	{ "conv.ovf.i.un",	ILL_KW },
	{ "conv.ovf.i1",	ILL_KW },
	{ "conv.ovf.i1.un",	ILL_KW },
	{ "conv.ovf.i2",	ILL_KW },
	{ "conv.ovf.i2.un",	ILL_KW },
	{ "conv.ovf.i4",	ILL_KW },
	{ "conv.ovf.i4.un",	ILL_KW },
	{ "conv.ovf.i8",	ILL_KW },
	{ "conv.ovf.i8.un",	ILL_KW },
	{ "conv.ovf.u",		ILL_KW },
	{ "conv.ovf.u.un",	ILL_KW },
	{ "conv.ovf.u1",	ILL_KW },
	{ "conv.ovf.u1.un",	ILL_KW },
	{ "conv.ovf.u2",	ILL_KW },
	{ "conv.ovf.u2.un",	ILL_KW },
	{ "conv.ovf.u4",	ILL_KW },
	{ "conv.ovf.u4.un",	ILL_KW },
	{ "conv.ovf.u8",	ILL_KW },
	{ "conv.ovf.u8.un",	ILL_KW },
	{ "conv.r.un",		ILL_KW },
	{ "conv.r4",		ILL_KW },
	{ "conv.r8",		ILL_KW },
	{ "conv.u",			ILL_KW },
	{ "conv.u1",		NEED_NOTHING },
	{ "conv.u2",		NEED_NOTHING },
	{ "conv.u4",		NEED_NOTHING },
	{ "conv.u8",		NEED_NOTHING },
	{ "cpblk",			NEED_NOTHING },
	{ "cpobj",			ILL_KW },
	{ "currency",		ILL_KW },
	{ "custom",			ILL_KW },
	{ "date",			ILL_KW },
	{ "decimal",		ILL_KW },
	{ "default",		ILL_KW },
	{ "demand",			ILL_KW },
	{ "deny",			ILL_KW },
	{ "div",			NEED_NOTHING },
	{ "div.un",			NEED_NOTHING },
	{ "dup",			NEED_NOTHING },
	{ "endfault",		ILL_KW },
	{ "endfilter",		ILL_KW },
	{ "endfinally",		ILL_KW },
	{ "endmac",			ILL_KW },
	{ "enum",			ILL_KW },
	{ "error",			ILL_KW },
	{ "explicit",		ILL_KW },
	{ "extends",		ILL_KW },
	{ "extern",			ILL_KW },
	{ "false",			ILL_KW },
	{ "famandassem",	ILL_KW },
	{ "family",			ILL_KW },
	{ "famorassem",		ILL_KW },
	{ "fastcall",		ILL_KW },
	{ "fault",			ILL_KW },
	{ "field",			ILL_KW },
	{ "filetime",		ILL_KW },
	{ "filter",			ILL_KW },
	{ "final",			ILL_KW },
	{ "finally",		ILL_KW },
	{ "fixed",			ILL_KW },
	{ "float",			ILL_KW },
	{ "float32",		ILL_KW },
	{ "float64",		ILL_KW },
	{ "forwardref",		ILL_KW },
	{ "fromunmanaged",	ILL_KW },
	{ "handler",		ILL_KW },
	{ "hidebysig",		ILL_KW },
	{ "hresult",		ILL_KW },
	{ "idispatch",		ILL_KW },
	{ "il",				ILL_KW },
	{ "illegal",		ILL_KW },
	{ "implements",		ILL_KW },
	{ "implicitcom",	ILL_KW },
	{ "implicitres",	ILL_KW },
	{ "import",			ILL_KW },
	{ "in",				ILL_KW },
	{ "inheritcheck",	ILL_KW },
	{ "init",			ILL_KW },
	{ "initblk",		NEED_NOTHING },
	{ "initobj",		ILL_KW },
	{ "initonly",		ILL_KW },
	{ "instance",		ILL_KW },
	{ "int",			ILL_KW },
	{ "int16",			ILL_KW },
	{ "int32",			ILL_KW },
	{ "int64",			ILL_KW },
	{ "int8",			ILL_KW },
	{ "interface",		ILL_KW },
	{ "internalcall",	ILL_KW },
	{ "isinst",			ILL_KW },
	{ "iunknown",		ILL_KW },
	{ "jmp",			ILL_KW },
	{ "lasterr",		ILL_KW },
	{ "lcid",			ILL_KW },
	{ "ldarg",			NEED_NUMBER },
	{ "ldarg.0",		NEED_NOTHING },
	{ "ldarg.1",		NEED_NOTHING },
	{ "ldarg.2",		NEED_NOTHING },
	{ "ldarg.3",		NEED_NOTHING },
	{ "ldarg.s",		NEED_NUMBER },
	{ "ldarga",			ILL_KW },
	{ "ldarga.s",		ILL_KW },
	// 又是九个没有的, 微软方言
	// nine don't have again, microsoft dialect.
	{ "ldc.i.0",		NEED_0 },
	{ "ldc.i.1",		NEED_1 },
	{ "ldc.i.2",		NEED_2 },
	{ "ldc.i.3",		NEED_3 },
	{ "ldc.i.4",		NEED_4 },
	{ "ldc.i.5",		NEED_5 },
	{ "ldc.i.6",		NEED_6 },
	{ "ldc.i.7",		NEED_7 },
	{ "ldc.i.8",		NEED_8 },
	{ "ldc.i4",			NEED_SNUMBER },
	{ "ldc.i4.0",		ILL_KW },
	{ "ldc.i4.1",		ILL_KW },
	{ "ldc.i4.2",		ILL_KW },
	{ "ldc.i4.3",		ILL_KW },
	{ "ldc.i4.4",		ILL_KW },
	{ "ldc.i4.5",		ILL_KW },
	{ "ldc.i4.6",		ILL_KW },
	{ "ldc.i4.7",		ILL_KW },
	{ "ldc.i4.8",		ILL_KW },
	{ "ldc.i4.M1",		ILL_KW },
	{ "ldc.i4.m1",		ILL_KW },
	{ "ldc.i4.s",		NEED_SNUMBER },
	{ "ldc.i8",			ILL_KW },
	{ "ldc.r4",			ILL_KW },
	{ "ldc.r8",			ILL_KW },
	{ "ldelem",			ILL_KW },
	{ "ldelem.i",		ILL_KW },
	{ "ldelem.i1",		ILL_KW },
	{ "ldelem.i2",		ILL_KW },
	{ "ldelem.i4",		ILL_KW },
	{ "ldelem.i8",		ILL_KW },
	{ "ldelem.r4",		ILL_KW },
	{ "ldelem.r8",		ILL_KW },
	{ "ldelem.ref",		ILL_KW },
	{ "ldelem.u1",		ILL_KW },
	{ "ldelem.u2",		ILL_KW },
	{ "ldelem.u4",		ILL_KW },
	{ "ldelem.u8",		ILL_KW },
	{ "ldelema",		ILL_KW },
	{ "ldfld",			ILL_KW },
	{ "ldflda",			ILL_KW },
	{ "ldftn",			ILL_KW },
	{ "ldind.i",		ILL_KW },
	{ "ldind.i1",		NEED_NOTHING },
	{ "ldind.i2",		NEED_NOTHING },
	{ "ldind.i4",		NEED_NOTHING },
	{ "ldind.i8",		NEED_NOTHING },
	{ "ldind.r4",		ILL_KW },
	{ "ldind.r8",		ILL_KW },
	{ "ldind.ref",		ILL_KW },
	{ "ldind.u1",		NEED_NOTHING },
	{ "ldind.u2",		NEED_NOTHING },
	{ "ldind.u4",		NEED_NOTHING },
	{ "ldind.u8",		NEED_NOTHING },
	{ "ldlen",			ILL_KW },
	{ "ldloc",			NEED_NUMBER },
	{ "ldloc.0",		NEED_NOTHING },
	{ "ldloc.1",		NEED_NOTHING },
	{ "ldloc.2",		NEED_NOTHING },
	{ "ldloc.3",		NEED_NOTHING },
	{ "ldloc.s",		NEED_NUMBER },
	{ "ldloca",			NEED_NUMBER },
	{ "ldloca.s",		NEED_NUMBER },
	{ "ldnull",			ILL_KW },
	{ "ldobj",			ILL_KW },
	{ "ldsfld",			NEED_LABEL },
	{ "ldsflda",		NEED_LABEL },
	{ "ldstr",			ILL_KW },
	{ "ldtoken",		ILL_KW },
	{ "ldvirtftn",		ILL_KW },
	{ "leave",			ILL_KW },
	{ "leave.s",		ILL_KW },
	{ "linkcheck",		ILL_KW },
	{ "literal",		ILL_KW },
	{ "localloc",		ILL_KW },
	{ "lpstr",			ILL_KW },
	{ "lpstruct",		ILL_KW },
	{ "lptstr",			ILL_KW },
	{ "lpvoid",			ILL_KW },
	{ "lpwstr",			ILL_KW },
	{ "managed",		ILL_KW },
	{ "marshal",		ILL_KW },
	{ "method",			ILL_KW },
	{ "mkrefany",		ILL_KW },
	{ "modopt",			ILL_KW },
	{ "modreq",			ILL_KW },
	{ "mul",			NEED_NOTHING },
	{ "mul.ovf",		ILL_KW },
	{ "mul.ovf.un",		ILL_KW },
	{ "native",			ILL_KW },
	{ "neg",			ILL_KW },
	{ "nested",			ILL_KW },
	{ "newarr",			ILL_KW },
	{ "newobj",			ILL_KW },
	{ "newslot",		ILL_KW },
	{ "no.",			ILL_KW },
	{ "noappdomain",	ILL_KW },
	// 又是一个没有的, 微软方言
	// a don't have again, microsoft dialect.
	{ "noduplicate",	ILL_KW },
	{ "noinlining",		ILL_KW },
	{ "nomachine",		ILL_KW },
	{ "nomangle",		ILL_KW },
	{ "nometadata",		ILL_KW },
	{ "noncasdemand",	ILL_KW },
	{ "noncasinheritance",	ILL_KW },
	{ "noncaslinkdemand",	ILL_KW },
	{ "nop",			ILL_KW },
	{ "noprocess",		ILL_KW },
	{ "not",			NEED_NOTHING },
	{ "not_in_gc_heap",	ILL_KW },
	{ "notremotable",	ILL_KW },
	{ "notserialized",	ILL_KW },
	{ "null",			ILL_KW },
	{ "nullref",		ILL_KW },
	{ "object",			ILL_KW },
	{ "objectref",		ILL_KW },
	{ "opt",			ILL_KW },
	{ "optil",			ILL_KW },
	{ "or",				NEED_NOTHING },
	{ "out",			ILL_KW },
	{ "permitonly",		ILL_KW },
	{ "pinned",			ILL_KW },
	{ "pinvokeimpl",	ILL_KW },
	{ "pop",			NEED_NOTHING },
	{ "prefix1",		ILL_KW },
	{ "prefix2",		ILL_KW },
	{ "prefix3",		ILL_KW },
	{ "prefix4",		ILL_KW },
	{ "prefix5",		ILL_KW },
	{ "prefix6",		ILL_KW },
	{ "prefix7",		ILL_KW },
	{ "prefixref",		ILL_KW },
	{ "prejitdeny",		ILL_KW },
	{ "prejitgrant",	ILL_KW },
	{ "preservesig",	ILL_KW },
	{ "private",		ILL_KW },
	{ "privatescope",	ILL_KW },
	{ "protected",		ILL_KW },
	{ "public",			ILL_KW },
	{ "readonly.",		ILL_KW },
	{ "record",			ILL_KW },
	{ "refany",			ILL_KW },
	{ "refanytype",		ILL_KW },
	{ "refanyval",		ILL_KW },
	{ "rem",			NEED_NOTHING },
	{ "rem.un",			NEED_NOTHING },
	{ "reqmin",			ILL_KW },
	{ "reqopt",			ILL_KW },
	{ "reqrefuse",		ILL_KW },
	{ "reqsecobj",		ILL_KW },
	{ "request",		ILL_KW },
	{ "ret",			NEED_NOTHING },
	{ "rethrow",		ILL_KW },
	{ "retval",			ILL_KW },
	{ "rtspecialname",	ILL_KW },
	{ "runtime",		ILL_KW },
	{ "safearray",		ILL_KW },
	{ "sealed",			ILL_KW },
	{ "sequential",		ILL_KW },
	{ "serializable",	ILL_KW },
	{ "shl",			NEED_NOTHING },
	{ "shr",			NEED_NOTHING },
	{ "shr.un",			NEED_NOTHING },
	{ "sizeof",			ILL_KW },
	{ "special",		ILL_KW },
	{ "specialname",	ILL_KW },
	{ "starg",			NEED_NUMBER },
	{ "starg.s",		NEED_NUMBER },
	{ "static",			ILL_KW },
	{ "stdcall",		ILL_KW },
	{ "stelem",			ILL_KW },
	{ "stelem.i",		ILL_KW },
	{ "stelem.i1",		ILL_KW },
	{ "stelem.i2",		ILL_KW },
	{ "stelem.i4",		ILL_KW },
	{ "stelem.i8",		ILL_KW },
	{ "stelem.r4",		ILL_KW },
	{ "stelem.r8",		ILL_KW },
	{ "stelem.ref",		ILL_KW },
	{ "stfld",			ILL_KW },
	{ "stind.i",		ILL_KW },
	{ "stind.i1",		NEED_NOTHING },
	{ "stind.i2",		NEED_NOTHING },
	{ "stind.i4",		NEED_NOTHING },
	{ "stind.i8",		NEED_NOTHING },
	{ "stind.r4",		ILL_KW },
	{ "stind.r8",		ILL_KW },
	{ "stind.ref",		ILL_KW },
	{ "stloc",			NEED_NUMBER },
	{ "stloc.0",		NEED_NOTHING },
	{ "stloc.1",		NEED_NOTHING },
	{ "stloc.2",		NEED_NOTHING },
	{ "stloc.3",		NEED_NOTHING },
	{ "stloc.s",		NEED_NUMBER },
	{ "stobj",			ILL_KW },
	{ "storage",		ILL_KW },
	{ "stored_object",	ILL_KW },
	{ "stream",			ILL_KW },
	{ "streamed_object",	ILL_KW },
	{ "string",			ILL_KW },
	{ "struct",			ILL_KW },
	{ "stsfld",			NEED_LABEL },
	{ "sub",			NEED_NOTHING },
	{ "sub.ovf",		ILL_KW },
	{ "sub.ovf.un",		ILL_KW },
	{ "switch",			NEED_NUMBER },
	{ "synchronized",	ILL_KW },
	{ "syschar",		ILL_KW },
	{ "sysstring",		ILL_KW },
	{ "tail.",			ILL_KW },
	{ "tbstr",			ILL_KW },
	{ "thiscall",		ILL_KW },
	{ "throw",			ILL_KW },
	{ "tls",			ILL_KW },
	{ "to",				ILL_KW },
	{ "true",			ILL_KW },
	{ "typedref",		ILL_KW },
	// 这个是被改过的, 微软方言
	// this have been changed, microsoft dialect.
	{ "unaligned",		NEED_NUMBER },
//	{ "unaligned.",		ILL_KW },
	{ "unbox",			ILL_KW },
	{ "unbox.any",		ILL_KW },
	{ "unicode",		ILL_KW },
	{ "unmanaged",		ILL_KW },
	{ "unmanagedexp",	ILL_KW },
	{ "unsigned",		ILL_KW },
	{ "unused",			ILL_KW },
	{ "userdefined",	ILL_KW },
	{ "value",			ILL_KW },
	{ "valuetype",		ILL_KW },
	{ "vararg",			ILL_KW },
	{ "variant",		ILL_KW },
	{ "vector",			ILL_KW },
	{ "virtual",		ILL_KW },
	{ "void",			ILL_KW },
	{ "volatile.",		ILL_KW },
	{ "wchar",			ILL_KW },
	{ "winapi",			ILL_KW },
	{ "with",			ILL_KW },
	{ "wrapper",		ILL_KW },
	{ "xor",			NEED_NOTHING },
};

#define WORD_MAX	4096

__i16	word_buff[WORD_MAX];
__i16	word_pos;

static void conv_word_buff(void)
{
	__i8	*p;
	__i16	i;

	p = (__i8 *)&word_buff;
	i = 0;
	while (word_buff[i])
	{
		p[i] = (__i8)word_buff[i];
		i++;
	}
	p[i] = 0;
}

static __i16	curchar;
static __i32	lines;
static __i16	indexes;

static void init_lex(void)
{
	lines = 1;
	curchar = getnextchar();
}

#define		KEYWORD			1000
#define		LABEL			1001
#define		DIRECTIVE		1002
#define		QSTRING			1003
#define		NUMBER			1004
#define		SYMBOL_MINUS	1005
#define		SYMBOL_COMMA	1006
#define		SYMBOL_COLON	1007
#define		OTHER			1008
#define		UNKNOWN_CHAR	1009

#include <string.h>

static __i16 getnextword(void)
{
	__i8	*p;
	__i16	l, r, m;
	__i8	matched;

	while ( curchar != CHAR_EOF )
	{
		if (curchar & (__i16)0xFF80)
		{
			fprintf(stderr, "\nError [%s][%d]: not ascii char detect!\n", infname, lines);
			return STATUS_ERR;
		}
		if (curchar == '\r' || curchar == '\t' || curchar == ' ' ) // 空白字符  space
		{
			curchar = getnextchar();
			continue; 
		}
		if ( curchar == '\n' ) // 行结束 line end
		{ 
			lines ++; 
			curchar = getnextchar();
			continue;
		}
		if (curchar == ';') // 注释 comments
		{
			do{ curchar = getnextchar(); }
			while ( curchar != '\n' && curchar != CHAR_EOF );
			if ( curchar == '\n' )
			{
				lines ++;
				curchar = getnextchar();
			}
			continue;
		}
		if ( curchar == '.' ) // 伪指令 directive
		{
			word_buff[0] = curchar;
			word_pos = 1;
			curchar = getnextchar();
			while (curchar >= 'a' && curchar <= 'z' )
			{
				word_buff[word_pos] = curchar;
				word_pos ++;
				curchar = getnextchar();
			}
			word_buff[word_pos] = 0;
			conv_word_buff();
			// 二分查找
			// binary chop
			l  = 0; r = MAX_DIRECTIVES - 1;
			matched = 0;
			while (l < r)
			{
				m = (l + r)/2;
				matched = strcmp((const char *)&word_buff, (const char *)directives[m].keyword);
				if ( matched == 0 ) { matched = 1; break; }
				if ( matched > 0) { l = m + 1; }
				else { r = m - 1; }
				matched = 0;
			}
			if (!matched)
			{
				if ( l == r )
				{
					m = l;
					if ( strcmp((const char *)&word_buff, (const char *)directives[m].keyword) == 0 )
					{ matched  = 1; }
					else { matched = 0; }
				}
			}
			if (!matched)
			{
				// 又是一个没有的, 而且还很特殊, 微软方言
				// a don't have again, and very special, microsoft dialect.
				if ( word_pos == 1 && curchar == 'C' )
				{
					p = (__i8 *)&word_buff;
					while ( curchar >= 'A' && curchar <= 'T' )
					{
						p[word_pos] = (__i8) curchar;
						word_pos ++;
						curchar = getnextchar();
					}
					p[word_pos] = 0;
					if (strcmp((const char *)&word_buff, ".CRTMA"))
					{
						fprintf(stderr, "\nGrammar Error [%s][%d]: [%s]!\n", infname, lines, word_buff);
						return STATUS_ERR;
					}
					return DIRECTIVE_OTHER;
				}
				fprintf(stderr, "\nError [%s][%d]: Unknown directive [%s]!\n", infname, lines, word_buff);
				return STATUS_ERR;
			}
			indexes = m;
			return DIRECTIVE;
		}
		if ( curchar == '^' ) // "^THE_END^"
		{
			word_buff[0] = curchar;
			word_pos = 1;
			curchar = getnextchar();
			while (curchar >= 'D' && curchar <= '_' )
			{
				word_buff[word_pos] = curchar;
				word_pos ++;
				curchar = getnextchar();
			}
			word_buff[word_pos] = 0;
			conv_word_buff();
			if (strcmp((const char *)&word_buff, (const char *)THE_END))
			{
				fprintf(stderr, "\nGrammar Error [%s][%d]: [%s]!\n", infname, lines, word_buff);
				return STATUS_ERR;
			}
			return OTHER;
		}
		if ( curchar >= 'a' && curchar <= 'z' ) // 关键字
		{
			word_buff[0] = curchar;
			word_pos = 1;
			curchar = getnextchar();
			while (curchar >= 'a' && curchar <= 'z' || curchar >= '0' && curchar <= '8' || curchar == '.' || curchar == 'M' )
			{
				word_buff[word_pos] = curchar;
				word_pos ++;
				curchar = getnextchar();
			}
			word_buff[word_pos] = 0;
			conv_word_buff();
			// 二分查找
			// binary chop
			l  = 0; r = MAX_KEYWORDS - 1;
			matched = 0;
			while (l < r)
			{
				m = (l + r)/2;
				matched = strcmp((const char *)&word_buff, (const char *)keywords[m].keyword);
				if ( matched == 0 ) { matched = 1; break; }
				if ( matched > 0) { l = m + 1; }
				else { r = m - 1; }
				matched = 0;
			}
			if (!matched)
			{
				if ( l == r )
				{
					m = l;
					if ( strcmp((const char *)&word_buff, (const char *)keywords[m].keyword) == 0 )
					{ matched  = 1; }
					else { matched = 0; }
				}
			}
			if (!matched)
			{
				p = (__i8 *)&word_buff;
				// 又发现一个没有的关键字, 而且还很特殊, 微软方言
				// ldc.i.-1		什么鬼语法!
				// a don't have keyword found again, and very special, microsoft dialect.
				// ldc.i.-1		what a GOD damn slang!
				if (strcmp((const char *)&word_buff, "ldc.i.") == 0 && curchar == '-' ) 
				{
					p[word_pos] = (__i8) curchar;
					word_pos ++;
					curchar = getnextchar();
					if (curchar == '1' )
					{
						p[word_pos] = (__i8)curchar;
						curchar = getnextchar();
						word_pos ++;
						p[word_pos] = 0;
						return KEYWORD_OTHER;
					}
				}
				// 这有可能是一个标号, 查一查看它是否符合标号的定义
				// it may be a label, take a look whether it is suit for the definition of label.
				if ( p[word_pos - 1] == '-' ) { matched = 1; }
				else
				{
					for (l = 0; l < word_pos; l++)
					{
						if ( p[l] == '.' )
						{ matched = 1; break; }
					}
				}
				if (matched)
				{
					fprintf(stderr, "\nGrammar Error [%s][%d]: [%s]!\n", infname, lines, word_buff);
					return STATUS_ERR;
				}
				// 这确实是一个标号, 但它有可能与关键字相冲突
				// it is indeed a label, but may conflict with keywords
				while ( curchar >= 'a' && curchar <= 'z' || curchar >= 'A' && curchar <= 'Z' ||curchar >= '0' && curchar <= '9' ||
						curchar == '_' || curchar == '?' || curchar == '$' || curchar == '@' )
				{
					if ( curchar == '_' || curchar == '?' || curchar == '$' || curchar == '@' )
					{ matched = 1; }
					p[word_pos] = (__i8) curchar;
					word_pos ++;
					curchar = getnextchar();
				}
				p[word_pos] = 0;
				if (!matched)
				{
					// 二分查找
					// binary chop
					l  = 0; r = MAX_KEYWORDS - 1;
					matched = 0;
					while (l < r)
					{
						m = (l + r)/2;
						matched = strcmp((const char *)&word_buff, (const char *)keywords[m].keyword);
						if ( matched == 0 ) { matched = 1; break; }
						if ( matched > 0) { l = m + 1; }
						else { r = m - 1; }
						matched = 0;
					}
					if (!matched)
					{
						if ( l == r )
						{
							if ( strcmp((const char *)&word_buff, (const char *)keywords[l].keyword) == 0 )
							{ matched  = 1; }
							else { matched = 0; }
						}
					}
					if ( matched )
					{
						fprintf(stderr, "\nError [%s][%d]: This Label conflict with keywords: [%s]!\n", infname, lines, word_buff);
						return STATUS_ERR;
					}
				}
				// 这是一个合法的标号
				// it is a legal label.
				return LABEL;
			}
			indexes = m;
			return KEYWORD;
		}
		if ( curchar >= 'A' && curchar <= 'Z' || curchar == '_' || curchar == '?' || curchar == '$' || curchar == '@' ) // 标号 label
		{
			word_buff[0] = curchar;
			word_pos = 1;
			// 这个标号有可能与关键字相冲突
			// this label may conflict with keywords.
			if ( curchar == '_' || curchar == '?' || curchar == '$' || curchar == '@' )
			{ matched = 1; } else { matched = 0; }
			curchar = getnextchar();
			while ( curchar >= 'a' && curchar <= 'z' || curchar >= 'A' && curchar <= 'Z' ||curchar >= '0' && curchar <= '9' ||
					curchar == '_' || curchar == '?' || curchar == '$' || curchar == '@' )
			{
				if ( !matched )
				{
					if ( curchar == '_' || curchar == '?' || curchar == '$' || curchar == '@' )
					{ matched = 1; }
				}
				word_buff[word_pos] = curchar;
				word_pos ++;
				curchar = getnextchar();
			}
			word_buff[word_pos] = 0;
			conv_word_buff();
			if (!matched)
			{
				// 二分查找
				// binary chop
				l  = 0; r = MAX_KEYWORDS - 1;
				matched = 0;
				while (l < r)
				{
					m = (l + r)/2;
					matched = strcmp((const char *)&word_buff, (const char *)keywords[m].keyword);
					if ( matched == 0 ) { matched = 1; break; }
					if ( matched > 0) { l = m + 1; }
					else { r = m - 1; }
					matched = 0;
				}
				if (!matched)
				{
					if ( l == r )
					{
						if ( strcmp((const char *)&word_buff, (const char *)keywords[l].keyword) == 0 )
						{ matched  = 1; }
						else { matched = 0; }
					}
				}
				if ( matched )
				{
					fprintf(stderr, "\nError [%s][%d]: This Label conflict with keywords: [%s]!\n", infname, lines, word_buff);
					return STATUS_ERR;
				}
			}
			// 又发现两个没有的关键字, 而且还很特殊, 微软方言
			// two don't have keyword found again, and very special, microsoft dialect.
			if (strcmp((const char *)&word_buff, "DD") == 0)
			{ return KEYWORD_OTHER_1; }
			else { if (strcmp((const char *)&word_buff, "_case") == 0) { return KEYWORD_OTHER_2; } }
			// 这是一个合法的标号
			// it is a legal label.
			return LABEL;
		}
		if ( curchar == '"') // 双引号字符串 double quoted string
		{
			word_pos = 0;
			curchar = getnextchar();
			while (curchar != '"' )
			{
				word_buff[word_pos] = curchar;
				word_pos ++;
				// 又是一个不合乎 ECMA-335 标准的语法, 微软方言
				// again a not ECMA-335 standard syntax, microsoft dialect.
				if (curchar == '\\')
				{
					curchar = getnextchar();
					word_buff[word_pos] = curchar;
					word_pos ++;
				}
				curchar = getnextchar();
			}
			word_buff[word_pos] = 0;
			conv_word_buff();
			curchar = getnextchar();
			return QSTRING;
		}
		if ( curchar >= '0' && curchar <= '9' ) // 数字 number
		{
			word_buff[0] = curchar;
			word_pos = 1;
			curchar = getnextchar();
			while ( curchar >= '0' && curchar <= '9' )
			{
				word_buff[word_pos] = curchar;
				word_pos ++;
				curchar = getnextchar();
			}
			word_buff[word_pos] = 0;
			conv_word_buff();
			return NUMBER;
		}
		if ( curchar == '-' || curchar == ',' || curchar == ':' ) // 符号 symbol
		{
			p = (__i8 *)&word_buff;
			*p = (__i8)curchar;
			p[1] = 0;
			word_pos = 1;
			curchar = getnextchar();
			if ( *p == '-' ) { return SYMBOL_MINUS; }
			else { if ( *p == ',' ) { return SYMBOL_COMMA; }
			else { return SYMBOL_COLON; } }
		}
		p = (__i8 *)&word_buff;
		p[0] = (__i8) curchar;
		p[1] = 0;
		word_pos = 1;
		return UNKNOWN_CHAR;
	}
	return STATUS_DONE;
}


/*  
		语法分析器
		syntactic parser
*/

static __i16	state;

__i16 start_parser(void)
{
	__i16	wtype;
	__i8	preread;

	state = BEGIN_PARSE;
	preread = 0;
	while ( state != END_PARSE )
	{
		switch ( state)
		{
		case BEGIN_PARSE:
			if (preread) { preread = 0; }
			else { wtype = getnextword(); }
			switch (wtype)
			{
			case KEYWORD:
				printf("[%s] ", keywords[indexes].keyword);
				state = keywords[indexes].action;
				break;
			case DIRECTIVE:
				printf("[%s] ", directives[indexes].keyword);
				state = directives[indexes].action;
				break;
			case LABEL:
				printf("[%s] ", word_buff);
				state = NEED_COLON;
				break;
			case KEYWORD_OTHER:
				printf("[ldc.i.-1] ");
				state = NEED_S1;
				break;
			case KEYWORD_OTHER_2:
				printf("[_case] ");
				state = NEED_LABEL;
				break;
			case KEYWORD_OTHER_1:
				printf("[DD] ");
				state = NEED_LABEL;
				break;
			case DIRECTIVE_OTHER:
				printf("[.CRTMA] ");
				state = NEED_LABEL;
				break;
			case UNKNOWN_CHAR:
				fprintf(stderr, "\nError [%s][%d]: unknown char: [%s]!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
				break;
			case STATUS_DONE:
			case STATUS_ERR:
				state = END_PARSE;
				break;
			default:
				fprintf(stderr, "\nError [%s][%d]: error type: [%d]!\n", infname, lines, wtype);
				wtype = STATUS_ERR;
				state = END_PARSE;
			};
			break;
		case NEED_0:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "0") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '0' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[0]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_1:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "1") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '1' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[1]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_2:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "2") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '2' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[2]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_3:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "3") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '3' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[3]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_4:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "4") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '4' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[4]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_5:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "5") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '5' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[5]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_6:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "6") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '6' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[6]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_7:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "7") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '7' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[7]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_8:
			wtype = getnextword();
			if ( wtype != NUMBER || strcmp((const char *)&word_buff, "8") )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '8' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[4]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_SNUMBER:
			wtype = getnextword();
			if ( wtype == SYMBOL_MINUS )
			{ 
				printf("[-] "); 
				wtype = getnextword();
			}
			if ( wtype != NUMBER )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: number is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[%s]\n", word_buff); state = BEGIN_PARSE; }
			break;
		case NEED_NUMBER:
			wtype = getnextword();
			if ( wtype != NUMBER )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: number is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[%s]\n", word_buff); state = BEGIN_PARSE; }
			break;
		case NEED_S1:
			wtype = getnextword();
			if (wtype != SYMBOL_MINUS)
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '-1' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[-] "); state = NEED_1; }
			break;
		case NEED_LABEL:
			wtype = getnextword();
			if (wtype != LABEL)
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: label is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[%s]\n", word_buff); state = BEGIN_PARSE; }
			break;
		case NEED_NOTHING:
			printf("\n");
			state = BEGIN_PARSE;
			break;
		case NEED_L_C_N:
			wtype = getnextword();
			if (wtype != LABEL)
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: label is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[%s] ", word_buff); state = NEED_COMMA_N; }
			break;
		case NEED_COMMA_N:
			wtype = getnextword();
			if (wtype != SYMBOL_COMMA)
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: ',' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[,] "); state = NEED_NUMBER; }
			break;
		case NEED_COLON:
			wtype = getnextword();
			if (wtype != SYMBOL_COLON)
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: ':' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[:]\n"); state = BEGIN_PARSE; }
			break;
		case NEED_QSTRING:
			wtype = getnextword();
			if (wtype != QSTRING)
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: QString is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else { printf("[%s]\n", word_buff); state = BEGIN_PARSE; }
			break;
		case COMPLEX_1:
			wtype = getnextword();
			if (wtype != QSTRING)
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: QString is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else {
				while (wtype == QSTRING)
				{
					printf("[%s] ", word_buff);
					wtype = getnextword();
				}
				preread = 1;
				printf("\n");
				state = BEGIN_PARSE; 
			}
			break;
		case NEED_TEXT_LABEL:
			wtype = getnextword();
			if (wtype != DIRECTIVE && indexes != INDEX_TEXT )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: '.text' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else
			{ 
				printf("[%s]", word_buff);
				state = NEED_LABEL;
			}
			break;
		case NEED_A_C_L:
			wtype = getnextword();
			if (wtype != KEYWORD && !(indexes == INDEX_ANY || indexes == INDEX_NODUPLICATE) )
			{
				fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: 'any' or 'noduplicate' is expected!\n", infname, lines, word_buff);
				wtype = STATUS_ERR;
				state = END_PARSE;
			}
			else
			{ 
				printf("[%s] ", word_buff);
				wtype = getnextword();
				if (wtype != SYMBOL_COMMA )
				{
					fprintf(stderr, "\nError [%s][%d]: syntax error [%s]: ',' is expected!\n", infname, lines, word_buff);
					wtype = STATUS_ERR;
					state = END_PARSE;
				}
				else
				{
					printf("[%s] ", word_buff);
					state = NEED_LABEL;
				}
			}
			break;
		case ILL_KW:
			fprintf(stderr, "\nError [%s][%d]: keyword [%s] is not support!\n", infname, lines, word_buff);
			wtype = STATUS_ERR;
			state = END_PARSE;
			break;
		default:
			// 这一情况应该永不出现
			// this situation shall never happen.
			fprintf(stderr, "\nError [%s][%d]: error state: [%d]!\n", infname, lines, state);
			wtype = STATUS_ERR;
			state = END_PARSE;
		};
	}
	return wtype;
}
