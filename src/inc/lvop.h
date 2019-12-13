#pragma once

// LVOP: opcode for local variable
//	op(4bit) src1(4bit) src2(4bit) dest(4bit)
//	example: ADD $1,$2,$3	($3=$1+$2)
//		equivalent to ADD | 0x0100 | 0x0020 | 0x0003 == 0x1123
enum class LVOP : uint32_t {
	opMask=0xFF000,

		NOP=0x00000,	// no operation

		ADD=0x01000,
		SUB=0x02000,
		MUL=0x03000,
		DIV=0x04000,
		MOD=0x05000,

		// dc: don't care
		INC=0x06000,	// INC lv,dc,lv		INC $1,,$2 ($2=$1+1)
		DEC=0x07000,
		TWC=0x08000,	// twice	TWC lv,dc,lv	TWC $3,,$4	($4=2*$3)
		HLF=0x09000,	// half		HLF lv,dc,lv	HLF $5,,$6	($6=$5/2)
		PW2=0x0A000,	// power2	PW2 lv,dc,lc	PW2 $5,,$7	($7=$5*$5)

	// stack: 0==DS, 1==RS
	TLV=0x20000,	// To Local Variable	TLV stack,dc,lv	LDS 0,,$1	($1=DS.Pop)
	RTL=0x21000,	// Read l &set To Local var.	RTL stack,dc,lv
					// RTL 1,,$2	($2=RS.ReadTOS)
	TST=0x23000,	// store To STack	TST $0,dc,stack	TST $0,,0	(DS.push($0))

	XPushBase=0x30000,	// ex: AddPush := XPushBase+ADD
	AddPush	 =0x31000,	// AddPush lv,lv,stack	AddPush $1,$2,ds	(DS.push($1+$2))
	SubPush	 =0x32000,
	MulPush	 =0x33000,
	DivPush	 =0x34000,
	ModPush	 =0x35000,

	IncPush  =0x36000,
	DecPush  =0x37000,
	TwcPush	 =0x38000,
	HlfPush  =0x39000,
	Pw2Push  =0x3A000,


	XPopBase =0x40000,
	PopAdd	 =0x41000,	// PopAdd stack,lv,lv	PopAdd ds,$1,$2	($2=DS.Pop+$1)
	PopSub	 =0x42000,
	PopMul	 =0x43000,
	PopDiv	 =0x44000,
	PopMod	 =0x45000,

	// ex: src1 local variable 2 (==$2) ==> s2
	// 	   src2 local variable 5 (==$5) ==> t5
	// 	   dest local variable 9 (==$9) ==> d9
	src1Mask=0x00F00,
	s0 =0x00000, s1 =0x00100, s2 =0x00200,  s3=0x00300,
	s4 =0x00400, s5 =0x00500, s6 =0x00600,  s7=0x00700,
	s8 =0x00800, s9 =0x00900, s10=0x00A00, s11=0x00B00,
	s12=0x00C00, s13=0x00D00, s14=0x00E00, s15=0x00F00,
	sDS=0x00000, sRS=0x00100,

	src2Mask=0x000F0,
	t0 =0x00000, t1 =0x00010, t2 =0x00020, t3 =0x00030,
	t4 =0x00040, t5 =0x00050, t6 =0x00060, t7 =0x00070,
	t8 =0x00080, t9 =0x00090, t10=0x000A0, t11=0x000B0,
	t12=0x000C0, t13=0x000D0, t14=0x000E0, t15=0x000F0,

	destMask=0x0000F,
	d0 =0x00000, d1 =0x00001, d2 =0x00002, d3 =0x00003,
	d4 =0x00004, d5 =0x00005, d6 =0x00006, d7 =0x00007,
	d8 =0x00008, d9 =0x00009, d10=0x0000A, d11=0x0000B,
	d12=0x0000C, d13=0x0000D, d14=0x0000E, d15=0x0000F,
	dDS=0x00000, dRS=0x00001,

	LVOpSupportedMask =0xF00000,
	LVOpSupported1args=0x100000,
	LVOpSupported2args=0x200000,
};

inline LVOP operator|(LVOP inOP1,LVOP inOP2) {
	return static_cast<LVOP>(
			static_cast<uint32_t>(inOP1) | static_cast<uint32_t>(inOP2)
		   );
}
inline LVOP operator&(LVOP inOP1,LVOP inOP2) {
	return static_cast<LVOP>(
			static_cast<uint32_t>(inOP1) & static_cast<uint32_t>(inOP2)
		   );
}
inline LVOP operator>>(LVOP inOP,int inShift) {
	return static_cast<LVOP>(
			static_cast<uint32_t>(inOP)>>inShift
		   );
}
inline bool operator!=(LVOP inOP,int inN) { return static_cast<int>(inOP)!=inN; }

inline std::string GetOpStr(LVOP inLVOP) {
	std::string ret;
	switch(inLVOP) {
		case LVOP::NOP:	ret="NOP";	break;
		case LVOP::ADD:	ret="ADD";	break;
		case LVOP::SUB:	ret="SUB";	break;
		case LVOP::MUL:	ret="MUL";	break;
		case LVOP::DIV:	ret="DIV";	break;
		case LVOP::MOD:	ret="MOD";	break;

		case LVOP::INC:	ret="INC";	break;
		case LVOP::DEC:	ret="DEC";	break;
		case LVOP::TWC:	ret="TWC";	break;
		case LVOP::HLF:	ret="HLF";	break;
		case LVOP::PW2:	ret="PW2";	break;

		case LVOP::TLV:	ret="TVL";	break;
		case LVOP::RTL:	ret="RTL";	break;
		case LVOP::TST:	ret="TST";	break;

		case LVOP::AddPush:	ret="AddPush";	break;
		case LVOP::SubPush:	ret="SubPush";	break;
		case LVOP::MulPush:	ret="MulPush";	break;
		case LVOP::DivPush:	ret="DivPush";	break;
		case LVOP::ModPush: ret="ModPush";	break;

		case LVOP::IncPush:	ret="IncPush";	break;
		case LVOP::DecPush: ret="DecPush";	break;
		case LVOP::TwcPush:	ret="TwcPush";	break;
		case LVOP::HlfPush:	ret="HlfPush";	break;
		case LVOP::Pw2Push:	ret="Pw2Push";	break;

		case LVOP::PopAdd:	ret="PopAdd";	break;
		case LVOP::PopSub:	ret="PopSub";	break;
		case LVOP::PopMul:	ret="PopMul";	break;
		case LVOP::PopDiv:	ret="PopDiv";	break;
		case LVOP::PopMod:	ret="PopMod";	break;

		default:
			fprintf(stderr,"SYSTEM ERROR in GetOpStr(LVOP). LVOP=%X\n",(unsigned int)inLVOP);
			exit(-1);
	}
	return ret;
}

inline std::string GetSrc1Str(LVOP inLVOP) {
	std::string ret;
	switch(inLVOP) {
		case LVOP::s0:	ret="$0";	break;
		case LVOP::s1:	ret="$1";	break;
		case LVOP::s2:	ret="$2";	break;
		case LVOP::s3:	ret="$3";	break;
		case LVOP::s4:	ret="$4";	break;
		case LVOP::s5:	ret="$5";	break;
		case LVOP::s6:	ret="$6";	break;
		case LVOP::s7:	ret="$7";	break;
		case LVOP::s8:	ret="$8";	break;
		case LVOP::s9:	ret="$9";	break;
		case LVOP::s10:	ret="$10";	break;
		case LVOP::s11:	ret="$11";	break;
		case LVOP::s12:	ret="$12";	break;
		case LVOP::s13:	ret="$13";	break;
		case LVOP::s14:	ret="$14";	break;
		case LVOP::s15:	ret="$15";	break;
		default:
			fprintf(stderr,"SYSTEM ERROR in GetSrc1Str(LVOP).\n");
			exit(-1);
	}
	return ret;
}

inline std::string GetSrc2Str(LVOP inLVOP) {
	std::string ret;
	switch(inLVOP) {
		case LVOP::t0:	ret="$0";	break;
		case LVOP::t1:	ret="$1";	break;
		case LVOP::t2:	ret="$2";	break;
		case LVOP::t3:	ret="$3";	break;
		case LVOP::t4:	ret="$4";	break;
		case LVOP::t5:	ret="$5";	break;
		case LVOP::t6:	ret="$6";	break;
		case LVOP::t7:	ret="$7";	break;
		case LVOP::t8:	ret="$8";	break;
		case LVOP::t9:	ret="$9";	break;
		case LVOP::t10:	ret="$10";	break;
		case LVOP::t11:	ret="$11";	break;
		case LVOP::t12:	ret="$12";	break;
		case LVOP::t13:	ret="$13";	break;
		case LVOP::t14:	ret="$14";	break;
		case LVOP::t15:	ret="$15";	break;
		default:
			fprintf(stderr,"SYSTEM ERROR in GetSrc2Str(LVOP).\n");
			exit(-1);
	}
	return ret;
}

inline std::string GetDestStr(LVOP inLVOP) {
	std::string ret;
	switch(inLVOP) {
		case LVOP::d0:	ret="$0";	break;
		case LVOP::d1:	ret="$1";	break;
		case LVOP::d2:	ret="$2";	break;
		case LVOP::d3:	ret="$3";	break;
		case LVOP::d4:	ret="$4";	break;
		case LVOP::d5:	ret="$5";	break;
		case LVOP::d6:	ret="$6";	break;
		case LVOP::d7:	ret="$7";	break;
		case LVOP::d8:	ret="$8";	break;
		case LVOP::d9:	ret="$9";	break;
		case LVOP::d10:	ret="$10";	break;
		case LVOP::d11:	ret="$11";	break;
		case LVOP::d12:	ret="$12";	break;
		case LVOP::d13:	ret="$13";	break;
		case LVOP::d14:	ret="$14";	break;
		case LVOP::d15:	ret="$15";	break;
		default:
			fprintf(stderr,"SYSTEM ERROR in GetDestStr(LVOP).\n");
			exit(-1);
	}
	return ret;
}

inline std::string GetFromStackStr(LVOP inLVOP) {
	std::string ret;
	switch(inLVOP) {
		case LVOP::sDS:	ret="DS";	break;
		case LVOP::sRS:	ret="RS";	break;
		default:
			fprintf(stderr,"SYSTEM ERROR in GetFromStackStr(LVOP).\n");
			exit(-1);
	}
	return ret;
}

inline std::string GetToStackStr(LVOP inLVOP) {
	std::string ret;
	switch(inLVOP) {
		case LVOP::dDS:	ret="DS";	break;
		case LVOP::dRS:	ret="RS";	break;
		default:
			fprintf(stderr,"SYSTEM ERROR in GetToStackStr(LVOP).\n");
			exit(-1);
	}
	return ret;
}

