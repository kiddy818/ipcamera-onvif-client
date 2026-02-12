#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <set>

// DeepSeek 问答：
// - 检查内容是否符合soap格式
// - C17编译发现std::regex找不到str()方法

class SOAPValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    // 主验证函数
    static ValidationResult validateSOAP(const std::string& soap_message) {
        ValidationResult result;
        result.valid = true;
        
        // 1. 检查XML声明
        if (!checkXMLDeclaration(soap_message)) {
            result.errors.push_back("缺少XML声明或格式错误");
            result.valid = false;
        }
        
        // 2. 检查SOAP Envelope
        if (!checkSoapEnvelope(soap_message)) {
            result.errors.push_back("SOAP Envelope格式错误");
            result.valid = false;
        }
        
        // 3. 检查SOAP Header (可选)
        auto header_check = checkSoapHeader(soap_message);
        if (!header_check.first) {
            result.warnings.push_back(header_check.second);
        }
        
        // 4. 检查SOAP Body
        if (!checkSoapBody(soap_message)) {
            result.errors.push_back("SOAP Body格式错误");
            result.valid = false;
        }
        
        // 5. 检查命名空间
        auto ns_check = checkNamespaces(soap_message);
        if (!ns_check.first) {
            result.errors.push_back(ns_check.second);
            result.valid = false;
        }
        
        // 6. 检查ONVIF特定要求
        auto onvif_check = checkONVIFRequirements(soap_message);
        if (!onvif_check.first) {
            result.warnings.push_back(onvif_check.second);
        }
        
        return result;
    }
    
private:
    // 检查XML声明
    static bool checkXMLDeclaration(const std::string& soap) {
        std::regex xml_decl_pattern(R"(<\?xml\s+version=\"1\.0\"\s+encoding=\"UTF-8\"\s*\?>)");
        return std::regex_search(soap, xml_decl_pattern);
    }
    
    // 检查SOAP Envelope
    static bool checkSoapEnvelope(const std::string& soap) {
        // 检查开始标签
        std::regex envelope_start(R"(<([^:>]*:)?Envelope)");
        // 检查结束标签  
        std::regex envelope_end(R"(</([^:>]*:)?Envelope>)");
        
        return std::regex_search(soap, envelope_start) && 
               std::regex_search(soap, envelope_end);
    }
    
    // 检查SOAP Header
    static std::pair<bool, std::string> checkSoapHeader(const std::string& soap) {
        std::regex header_pattern(R"(<([^:>]*:)?Header>.*</([^:>]*:)?Header>)");
        if (!std::regex_search(soap, header_pattern)) {
            return {false, "SOAP Header缺失（可选）"};
        }
        return {true, ""};
    }
    
    // 检查SOAP Body
    static bool checkSoapBody(const std::string& soap) {
        std::regex body_start(R"(<([^:>]*:)?Body>)");
        std::regex body_end(R"(</([^:>]*:)?Body>)");
        
        return std::regex_search(soap, body_start) && 
               std::regex_search(soap, body_end);
    }
    
    // 检查命名空间
    static std::pair<bool, std::string> checkNamespaces(const std::string& soap) {
        std::vector<std::pair<std::string, std::regex>> required_ns = {
            {"SOAP Envelope", std::regex(R"(xmlns:([^=]*)=[\"\']http://www\.w3\.org/2003/05/soap-envelope[\"\']")")},
            {"SOAP Envelope (1.1)", std::regex(R"(xmlns:([^=]*)=[\"\']http://schemas\.xmlsoap\.org/soap/envelope/[\"\']")")},
            {"ONVIF Device", std::regex(R"(xmlns:tds=[\"\']http://www\.onvif\.org/ver10/device/wsdl[\"\']")")}
        };
        
        for (const auto& ns : required_ns) {
            if (!std::regex_search(soap, ns.second)) {
                return {false, "缺少命名空间: " + ns.first};
            }
        }
        
        return {true, ""};
    }
    
    // 检查ONVIF特定要求
    static std::pair<bool, std::string> checkONVIFRequirements(const std::string& soap) {
        // 检查ONVIF特定的命名空间
        std::regex onvif_ns(R"(xmlns:([^=]*)=[\"\']http://www\.onvif\.org/ver[0-9]+/[^\"\']+[\"\'])");
        
        if (!std::regex_search(soap, onvif_ns)) {
            return {false, "缺少ONVIF命名空间"};
        }
        
        return {true, ""};
    }
};

class SOAPMessageValidator {
private:
    std::set<std::string> supported_soap_versions = {"1.1", "1.2"};
    
public:
    struct SOAPValidationDetail {
        bool has_xml_declaration;
        bool has_envelope;
        bool has_header;
        bool has_body;
        std::string soap_version;
        std::vector<std::string> namespaces;
        std::vector<std::string> errors;
        bool is_valid;
    };
    
    SOAPValidationDetail validate(const std::string& soap_message) {
        SOAPValidationDetail detail;
        detail.is_valid = true;
        
        // 1. XML声明检查
        detail.has_xml_declaration = checkXmlDeclaration(soap_message);
        if (!detail.has_xml_declaration) {
            detail.errors.push_back("❌ 缺少XML声明或格式不正确");
            detail.is_valid = false;
        }
        
        // 2. SOAP版本检测
        detail.soap_version = detectSoapVersion(soap_message);
        if (detail.soap_version == "unknown") {
            detail.errors.push_back("❌ 无法识别的SOAP版本");
            detail.is_valid = false;
        }
        
        // 3. Envelope检查
        detail.has_envelope = checkEnvelope(soap_message, detail.soap_version);
        if (!detail.has_envelope) {
            detail.errors.push_back("❌ SOAP Envelope格式错误");
            detail.is_valid = false;
        }
        
        // 4. Header检查
        detail.has_header = checkHeader(soap_message, detail.soap_version);
        
        // 5. Body检查
        detail.has_body = checkBody(soap_message, detail.soap_version);
        if (!detail.has_body) {
            detail.errors.push_back("❌ SOAP Body缺失");
            detail.is_valid = false;
        }
        
        // 6. 命名空间提取
        detail.namespaces = extractNamespaces(soap_message);
        
        // 7. 结构完整性检查
        if (!checkStructureIntegrity(soap_message)) {
            detail.errors.push_back("❌ XML结构不完整");
            detail.is_valid = false;
        }
        
        return detail;
    }
    
    void printValidationResult(const SOAPValidationDetail& detail) {
        std::cout << "════════════ SOAP消息验证结果 ════════════\n";
        std::cout << "XML声明: " << (detail.has_xml_declaration ? "✅ 正确" : "❌ 缺失") << std::endl;
        std::cout << "SOAP版本: " << detail.soap_version << std::endl;
        std::cout << "Envelope: " << (detail.has_envelope ? "✅ 存在" : "❌ 缺失") << std::endl;
        std::cout << "Header: " << (detail.has_header ? "✅ 存在" : "ℹ️ 可选/缺失") << std::endl;
        std::cout << "Body: " << (detail.has_body ? "✅ 存在" : "❌ 缺失") << std::endl;
        std::cout << "命名空间数量: " << detail.namespaces.size() << std::endl;
        
        if (!detail.namespaces.empty()) {
            std::cout << "命名空间列表:" << std::endl;
            for (const auto& ns : detail.namespaces) {
                std::cout << "  - " << ns << std::endl;
            }
        }
        
        if (!detail.errors.empty()) {
            std::cout << "\n❌ 错误列表:" << std::endl;
            for (const auto& error : detail.errors) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        std::cout << "\n整体验证结果: " << (detail.is_valid ? "✅ 有效" : "❌ 无效") << std::endl;
        std::cout << "═══════════════════════════════════════════\n";
    }
    
private:
    bool checkXmlDeclaration(const std::string& soap) {
        std::regex pattern(R"(<\?xml\s+version=[\"\']1\.0[\"\']\s+encoding=[\"\']UTF-8[\"\']\s*\?>)");
        return std::regex_search(soap, pattern);
    }
    
    std::string detectSoapVersion(const std::string& soap) {
        std::regex soap12(R"(http://www\.w3\.org/2003/05/soap-envelope)");
        std::regex soap11(R"(http://schemas\.xmlsoap\.org/soap/envelope/)");
        
        if (std::regex_search(soap, soap12)) return "1.2";
        if (std::regex_search(soap, soap11)) return "1.1";
        return "unknown";
    }
    
    bool checkEnvelope(const std::string& soap, const std::string& version) {
        std::regex envelope_pattern(R"(<([^:>]*:)?Envelope[^>]*>)");
        std::regex envelope_end_pattern(R"(</([^:>]*:)?Envelope>)");
        
        return std::regex_search(soap, envelope_pattern) && 
               std::regex_search(soap, envelope_end_pattern);
    }
    
    bool checkHeader(const std::string& soap, const std::string& version) {
        std::regex header_pattern(R"(<([^:>]*:)?Header[^>]*>.*</([^:>]*:)?Header>)");
        return std::regex_search(soap, header_pattern);
    }
    
    bool checkBody(const std::string& soap, const std::string& version) {
        std::regex body_pattern(R"(<([^:>]*:)?Body[^>]*>.*</([^:>]*:)?Body>)");
        return std::regex_search(soap, body_pattern);
    }
    
    std::vector<std::string> extractNamespaces(const std::string& soap) {
        std::vector<std::string> namespaces;
        std::regex ns_pattern(R"(xmlns:([^=]*)=[\"\'][^\"\']+[\"\'])");
        
        auto words_begin = std::sregex_iterator(soap.begin(), soap.end(), ns_pattern);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            namespaces.push_back((*i).str());
        }
        
        return namespaces;
    }
    
    bool checkStructureIntegrity(const std::string& soap) {
        // 简单的括号匹配
        int depth = 0;
        bool in_tag = false;
        
        for (char c : soap) {
            if (c == '<') {
                in_tag = true;
                depth++;
            } else if (c == '>') {
                in_tag = false;
                depth--;
            }
            
            if (depth < 0) return false;
        }
        
        return depth == 0;
    }
};

// C++20开始才有 std::regex 的 str()方法，使用替代方法实现
class NamedRegex {
public:
    NamedRegex(const std::string& pattern) 
        : m_pattern(pattern), m_regex(pattern) {}
    
    const std::string& pattern() const { return m_pattern; }
    const std::regex& regex() const { return m_regex; }
    
private:
    std::string m_pattern;
    std::regex m_regex;
};

class ONVIFSOAPValidator {
public:
    static bool validateGetDeviceInfoResponse(const std::string& soap_response) {
        std::vector<NamedRegex> required_elements = {
            NamedRegex(R"(<tds:GetDeviceInformationResponse[^>]*>)"),
            NamedRegex(R"(<tds:Manufacturer>.*</tds:Manufacturer>)"),
            NamedRegex(R"(<tds:Model>.*</tds:Model>)"),
            NamedRegex(R"(<tds:FirmwareVersion>.*</tds:FirmwareVersion>)"),
            NamedRegex(R"(<tds:SerialNumber>.*</tds:SerialNumber>)"),
            NamedRegex(R"(<tds:HardwareId>.*</tds:HardwareId>)")
        };
        
        for (const auto& element : required_elements) {
            if (!std::regex_search(soap_response, element.regex())) {
                std::cerr << "缺少必要元素: " << element.pattern() << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    static void prettyPrintSOAP(const std::string& soap) {
        // 提取SOAP结构
        std::regex envelope_pattern(R"(<([^:>]*:)?Envelope[^>]*>)");
        std::regex header_pattern(R"(<([^:>]*:)?Header[^>]*>)");
        std::regex body_pattern(R"(<([^:>]*:)?Body[^>]*>)");
        
        std::smatch match;
        
        std::cout << "📦 SOAP结构分析:\n";
        
        if (std::regex_search(soap, match, envelope_pattern))
            std::cout << "  ├─ " << match[0].str() << std::endl;
            
        if (std::regex_search(soap, match, header_pattern))
            std::cout << "  ├─ " << match[0].str() << std::endl;
            
        if (std::regex_search(soap, match, body_pattern))
            std::cout << "  └─ " << match[0].str() << std::endl;
    }
};
