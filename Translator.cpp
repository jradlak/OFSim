#include "Translator.h"

static inline std::string& ltrim(std::string& s);
static inline std::string& rtrim(std::string& s);
static inline std::string& trim(std::string& s);

void Translator::translate(const char* sourcePath)
{
    std::ifstream sourceFile;
    
    std::string sourceCode = "";

    sourceFile.open(sourcePath, std::ios::in);
        
    if (sourceFile.is_open()) {
        std::string line;
        
        while (std::getline(sourceFile, line))
        {
            recognizeInstr(line);
            findLabel(line);
        }

        //std::cout << "Wlasciwa translacja: \n";

        instr_addr = 0;
        sourceFile.clear();
        sourceFile.seekg(0);

        while (std::getline(sourceFile, line))
        {
            translate(line);
        }

        sourceFile.close();
    }
}

void Translator::translate(std::string sourceLine)
{
    std::tuple<unsigned int, unsigned int> instr = recognizeInstr(sourceLine);
    unsigned int opcode = std::get<0>(instr);
    if (opcode != 0x100)
    {
        switch (opcode)
        {          
        case 0x0: trnsl_mov(instr, sourceLine); break;
        case 0x1: trnsl_fmov(instr, sourceLine); break;
        case 0x3: trnsl_set(instr, sourceLine); break;
        case 0x4: trnsl_fset(instr, sourceLine); break;
        case 0x5: trnsl_ld(instr, sourceLine); break;
        case 0x6: trnsl_fld(instr, sourceLine); break;
        case 0x7: trnsl_bld(instr, sourceLine); break;
        case 0x8: trnsl_st(instr, sourceLine); break;
        case 0x9: trnsl_fst(instr, sourceLine); break;
        case 0xA: trnsl_bst(instr, sourceLine); break;
        case 0xB: trnsl_add(instr, sourceLine); break;
        case 0xC: trnsl_fadd(instr, sourceLine); break;
        case 0xD: trnsl_sub(instr, sourceLine); break;
        case 0xE: trnsl_fsub(instr, sourceLine); break;
        case 0xF: trnsl_mul(instr, sourceLine); break;
        case 0x10: trnsl_fmul(instr, sourceLine); break;
        case 0x11: trnsl_div(instr, sourceLine); break;
        case 0x12: trnsl_fdiv(instr, sourceLine); break;
        case 0x13: trnsl_mod(instr, sourceLine); break;
        case 0x14: trnsl_vor(instr, sourceLine); break;
        case 0x15: trnsl_vand(instr, sourceLine); break;
        case 0x16: trnsl_vxor(instr, sourceLine); break;
        case 0x17: trnsl_vnot(instr, sourceLine); break;
        case 0x18: trnsl_vshl(instr, sourceLine); break;
        case 0x19: trnsl_vshr(instr, sourceLine); break;
        case 0x1A: trnsl_cmp(instr, sourceLine); break;
        case 0x1B: trnsl_fcmp(instr, sourceLine); break;
        case 0x1C: trnsl_jz(instr, sourceLine); break;
        case 0x1D: trnsl_jnz(instr, sourceLine); break;
        case 0x1E: trnsl_jc(instr, sourceLine); break;
        case 0x1F: trnsl_jnc(instr, sourceLine); break;
        case 0x20: trnsl_jbe(instr, sourceLine); break;
        case 0x21: trnsl_ja(instr, sourceLine); break;
        case 0x22: trnsl_jmp(instr, sourceLine); break;
        case 0x23: trnsl_jmpr(instr, sourceLine); break;
        case 0x24: trnsl_halt(instr, sourceLine); break;       
        }
    }
}

void Translator::findLabel(std::string sourceLine)
{
    std::string line = trim(sourceLine);
    if ((int)line.rfind(":") > 0 && (int)line.rfind(";") < 0)
    {
        line = line.substr(0, line.size() - 1);
        labelDict.insert(std::pair<std::string, unsigned int>(line, instr_addr));
    }
}

void Translator::trnsl_mov(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    
}

void Translator::trnsl_fmov(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_set(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    line = trim(line);
    unsigned int opcode = std::get<0>(instr);
    unsigned int instrSize = std::get<1>(instr) + 1;
    
    // register number:
    int pos = line.find(" ") + 2;
    int pose = line.find(",");
    std::string regNumber = line.substr(pos, pose - pos);
    unsigned char reg = stoi(regNumber);

    // constant:
    pos = line.find(",") + 2;
    pose = line.size();
    std::string constant = line.substr(pos, pose - pos);
    unsigned int cnst = 0;
    if (std::isdigit(constant[0]))
    {
        cnst = stoi(constant);
    } 
    else
    {
        // label pointing to address
        cnst = labelDict[constant];
    }

    unsigned int addr = instr_addr - instrSize;
    code[addr++] = opcode;
    code[addr++] = reg;
    
    unsigned char* word = static_cast<unsigned char*>(static_cast<void*>(&cnst));
    Memory::memcopy(word, code, 0, addr, 4);
}

void Translator::trnsl_fset(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_ld(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    trnsl_vxor(instr, line);
}

void Translator::trnsl_fld(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    trnsl_vxor(instr, line);
}

void Translator::trnsl_bld(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    trnsl_vxor(instr, line);
}

void Translator::trnsl_st(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_fst(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_bst(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    trnsl_vxor(instr, line);
}

void Translator::trnsl_add(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    trnsl_vxor(instr, line);
}

void Translator::trnsl_fadd(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_sub(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_fsub(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_mul(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_fmul(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_div(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_fdiv(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_mod(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_vor(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_vand(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_vxor(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    line = trim(line);
    unsigned int opcode = std::get<0>(instr);
    unsigned int instrSize = std::get<1>(instr) + 1;

    // register number 1:
    int pos = line.find(" ") + 2;
    int pose = line.find(",");
    std::string regNumber = line.substr(pos, pose - pos);
    unsigned char reg1 = stoi(regNumber);

    // register number 2:
    pos = line.find(",") + 3;
    pose = line.size();
    regNumber = line.substr(pos, pose - pos);
    unsigned char reg2 = stoi(regNumber);

    unsigned int addr = instr_addr - instrSize;
    code[addr++] = opcode;
    code[addr++] = reg1;
    code[addr] = reg2;

    int a = 10;
}

void Translator::trnsl_vnot(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_vshl(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_vshr(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_cmp(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    trnsl_vxor(instr, line);
}

void Translator::trnsl_fcmp(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_jz(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    line = trim(line);
    unsigned int opcode = std::get<0>(instr);
    unsigned int instrSize = std::get<1>(instr) + 1;

    // constant:
    unsigned int pos = line.find(" ") + 1;
    unsigned int pose = line.size();
    std::string constant = line.substr(pos, pose - pos);
    unsigned int cnst = 0;
    if (std::isdigit(constant[0]))
    {
        cnst = stoi(constant);
    }
    else
    {
        // label pointing to address
        cnst = labelDict[constant];
    }

    unsigned int addr = instr_addr - instrSize;
    code[addr++] = opcode;
    
    unsigned char* word = static_cast<unsigned char*>(static_cast<void*>(&cnst));
    Memory::memcopy(word, code, 0, addr, 4);
}

void Translator::trnsl_jnz(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_jc(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_jnc(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_jbe(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_ja(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_jmp(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_jmpr(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

void Translator::trnsl_halt(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
}

std::tuple<unsigned int, unsigned int> Translator::recognizeInstr(std::string sourceLine)
{
    std::string line = trim(sourceLine);
    if ((int)line.rfind(";") < 0)
    {
        unsigned int pos = line.find(" ");
        std::string token = line.substr(0, pos);
        unsigned int opcode = opcodes->getOpcode(token);
        if (opcode != 0x100) 
        {
            unsigned int instrSize = opcodes->getInstrSize(opcode);
            instr_addr += instrSize + 0x1;
            return std::make_tuple(opcode, instrSize);
        }
    }

    return std::make_tuple(0x100, 0x0);
}

// code below based on: https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
// trim from start
static inline std::string& ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string& rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string& trim(std::string& s) {
    return ltrim(rtrim(s));
}