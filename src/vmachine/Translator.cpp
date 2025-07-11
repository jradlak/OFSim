#include "Translator.h"
#include "Memory.h"

#include <ostream>

static inline std::string& ltrim(std::string& s);
static inline std::string& rtrim(std::string& s);
static inline std::string& trim(std::string& s);

using namespace ofsim_vm;

Translator::Translator()
{    
    i18n = ofsim_gui::I18n::getInstance();
}

std::string Translator::translateSourceFile(const std::string sourcePath)
{
    std::ifstream file(sourcePath, std::ios::in);
    std::stringstream fileBuffer;
    fileBuffer << file.rdbuf();
    std::string fileData = fileBuffer.str();
    translateSourceString(fileData.c_str());
    file.close();

    return fileData;
}

void ofsim_vm::Translator::translateSourceString(const std::string sourceString)
{    
    std::string line;
    std::stringstream ss(sourceString.c_str());

    while (std::getline(ss, line))
    {
        recognizeInstr(line);
        findLabel(line);        
    }

    instr_addr = 0;
    int lineNumber = 1;
    std::stringstream ss2(sourceString);
    ss2.clear();
    ss2.seekg(0, std::ios::beg);
    while (std::getline(ss2, line))
    {
        std::cout << "Translating line " << lineNumber << ": " << line << std::endl;
        translateLine(line, lineNumber++);
    }
}

void Translator::translateLine(std::string sourceLine, int lineNumber)
{
    std::tuple<unsigned int, unsigned int> instr = recognizeInstr(sourceLine);
    unsigned int opcode = std::get<0>(instr);
    if (opcode != 0x100)
    {
        switch (opcode)
        {          
            case 0x0: trnsl_register_to_register(instr, sourceLine); break;
            case 0x1: trnsl_register_to_register(instr, sourceLine); break;
            case 0x3: trnsl_constant_to_register(instr, sourceLine); break;
            case 0x4: trnsl_fconstant_to_register(instr, sourceLine); break;
            case 0x5: trnsl_register_to_register(instr, sourceLine); break;
            case 0x6: trnsl_register_to_register(instr, sourceLine); break;
            case 0x7: trnsl_register_to_register(instr, sourceLine); break;
            case 0x8: trnsl_register_to_register(instr, sourceLine); break;
            case 0x9: trnsl_register_to_register(instr, sourceLine); break;
            case 0xA: trnsl_register_to_register(instr, sourceLine); break;
            case 0xB: trnsl_register_to_register(instr, sourceLine); break;
            case 0xC: trnsl_register_to_register(instr, sourceLine); break;
            case 0xD: trnsl_register_to_register(instr, sourceLine); break;
            case 0xE: trnsl_register_to_register(instr, sourceLine); break;
            case 0xF: trnsl_register_to_register(instr, sourceLine); break;
            case 0x10: trnsl_register_to_register(instr, sourceLine); break;
            case 0x11: trnsl_register_to_register(instr, sourceLine); break;
            case 0x12: trnsl_register_to_register(instr, sourceLine); break;
            case 0x13: trnsl_register_to_register(instr, sourceLine); break;
            case 0x14: trnsl_register_to_register(instr, sourceLine); break;
            case 0x15: trnsl_register_to_register(instr, sourceLine); break;
            case 0x16: trnsl_register_to_register(instr, sourceLine); break;
            case 0x17: trnsl_register(instr, sourceLine); break;
            case 0x18: trnsl_register_to_register(instr, sourceLine); break;
            case 0x19: trnsl_register_to_register(instr, sourceLine); break;
            case 0x1A: trnsl_register_to_register(instr, sourceLine); break;
            case 0x1B: trnsl_register_to_register(instr, sourceLine); break;
            case 0x1C: trnsl_constant(instr, sourceLine); break;
            case 0x1D: trnsl_constant(instr, sourceLine); break;
            case 0x1E: trnsl_constant(instr, sourceLine); break;
            case 0x1F: trnsl_constant(instr, sourceLine); break;
            case 0x20: trnsl_constant(instr, sourceLine); break;
            case 0x21: trnsl_constant(instr, sourceLine); break;
            case 0x22: trnsl_constant(instr, sourceLine); break;
            case 0x23: trnsl_register(instr, sourceLine); break;
            case 0x24: trnsl_halt(instr, sourceLine); break;
            case 0x25: trnsl_register_to_register(instr, sourceLine); break;
            case 0x26: trnsl_ftc(instr, sourceLine); break;
        }
    }
    else
    {
        // text data
        std::string line = trim(sourceLine);
        unsigned int pos = line.find(" ");
        std::string token = line.substr(0, pos);
        if (token == "db")
        {
            pos += 1;
            unsigned int pose = line.length() - pos;
            std::string dbData = line.substr(pos, pose);
            for (int i = 0; i < dbData.length(); i++)
            {
                unsigned char znk = dbData[i];
                code[instr_addr + i] = znk;
            }

            instr_addr += dbData.length();
        }
        else
        {
            // TODO: unrecognized token
            // TODO: error handling

            // std::string errMsg = i18n->t(translate_error);

            // errMsg = std::regex_replace(errMsg, std::regex("@lineNumber"), std::to_string(lineNumber));
            // errMsg = std::regex_replace(errMsg, std::regex("@lineTxt"), line);
           
            // std::cout << errMsg << "\n";
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

void Translator::trnsl_constant_to_register(std::tuple<unsigned int, unsigned int> instr, std::string line)
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

void Translator::trnsl_fconstant_to_register(std::tuple<unsigned int, unsigned int> instr, std::string line)
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
    double cnst = 0;
    if (std::isdigit(constant[0]) || constant[0] == '-')
    {
        cnst = stod(constant);
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
    Memory::memcopy(word, code, 0, addr, 8);
}

void Translator::trnsl_register_to_register(std::tuple<unsigned int, unsigned int> instr, std::string line)
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
}

void Translator::trnsl_constant(std::tuple<unsigned int, unsigned int> instr, std::string line)
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

void Translator::trnsl_register(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    line = trim(line);
    unsigned int opcode = std::get<0>(instr);
    unsigned int instrSize = std::get<1>(instr) + 1;

    // register :
    unsigned int pos = line.find(" ") + 2;
    unsigned int pose = line.size();
    std::string regNumber = line.substr(pos, pose - pos);
    unsigned char reg = stoi(regNumber);

    unsigned int addr = instr_addr - instrSize;
    code[addr++] = opcode;
    code[addr] = reg;    
}

void Translator::trnsl_halt(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    unsigned int opcode = std::get<0>(instr);
    unsigned int addr = instr_addr - 1;
    code[addr++] = opcode;
}

void ofsim_vm::Translator::trnsl_ftc(std::tuple<unsigned int, unsigned int> instr, std::string line)
{
    unsigned int opcode = std::get<0>(instr);   
    unsigned int addr = instr_addr - 1; 
    code[addr] = opcode;
}

std::tuple<unsigned int, unsigned int> Translator::recognizeInstr(std::string sourceLine)
{
    std::string line = trim(sourceLine);
    if ((int)line.rfind(";") < 0)
    {
        unsigned int pos = line.find(" ");
        std::string token = line.substr(0, pos);
        unsigned int opcode = opcodes.getOpcode(token);
        if (opcode != 0x100) 
        {
            unsigned int instrSize = opcodes.getInstrSize(opcode);
            instr_addr += instrSize + 0x1;
            return std::make_tuple(opcode, instrSize);
        }
    }

    return std::make_tuple(0x100, 0x0);
}

// code below based on: https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
// trim from start
static inline std::string& ltrim(std::string& s) 
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));

    return s;
}

// trim from end
static inline std::string& rtrim(std::string& s) 
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());

    return s;
}

// trim from both ends
static inline std::string& trim(std::string& s) {
    return ltrim(rtrim(s));
}
