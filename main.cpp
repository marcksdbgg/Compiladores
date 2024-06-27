#include "traduccion.h"

// Token
struct Token {
    std::string type;
    std::string value;
};

bool success = true;

std::vector<Token> tokens;

// Definicion de la estructura de produccion
struct Production {
    std::vector<std::string> symbols;
};

// Definicion del tipo de tabla de anolisis
typedef std::map<std::string, std::map<std::string, Production>> ParsingTable;

// Funcion para configurar la tabla de analisis sintactico
void setupParsingTable(ParsingTable& table) {
    // Producciones para S
    table["S"]["BEGIN"] = {{"Document", "$"}};
    table["S"]["HEADER"] = {{"Document", "$"}};
    table["S"]["BOLD"] = {{"Document", "$"}};
    table["S"]["ITALIC"] = {{"Document", "$"}};
    table["S"]["LINK"] = {{"Document", "$"}};
    table["S"]["IMAGE"] = {{"Document", "$"}};
    table["S"]["SPACE"] = {{"Document", "$"}};
    table["S"]["NEWLINE"] = {{"Document", "$"}};
    table["S"]["Text"] = {{"Document", "$"}};
    table["S"]["EOF"] = {{"Document", "$"}};

    // Producciones para Document
    table["Document"]["BEGIN"] = {{"Block", "Document"}};
    table["Document"]["HEADER"] = {{"Block", "Document"}};
    table["Document"]["BOLD"] = {{"Block", "Document"}};
    table["Document"]["ITALIC"] = {{"Block", "Document"}};
    table["Document"]["LINK"] = {{"Block", "Document"}};
    table["Document"]["IMAGE"] = {{"Block", "Document"}};
    table["Document"]["SPACE"] = {{"Block", "Document"}};
    table["Document"]["NEWLINE"] = {{"Block", "Document"}};
    table["Document"]["Text"] = {{"Block", "Document"}};
    table["Document"]["EOF"] = {{"EOF"}};

    // Producciones para Block
    table["Block"]["BEGIN"] = {{"CommandBlock"}};
    table["Block"]["HEADER"] = {{"CommandBlock"}};
    table["Block"]["BOLD"] = {{"CommandBlock"}};
    table["Block"]["ITALIC"] = {{"CommandBlock"}};
    table["Block"]["LINK"] = {{"CommandBlock"}};
    table["Block"]["IMAGE"] = {{"CommandBlock"}};
    table["Block"]["SPACE"] = {{"SPACE"}};
    table["Block"]["NEWLINE"] = {{"NEWLINE"}};
    table["Block"]["Text"] = {{"Text"}};

    // Producciones para CommandBlock
    table["CommandBlock"]["BEGIN"] = {{"BEGIN", "TableOrList"}};
    table["CommandBlock"]["HEADER"] = {{"HEADER", "Header"}};
    table["CommandBlock"]["BOLD"] = {{"BOLD", "StyledBlock"}};
    table["CommandBlock"]["ITALIC"] = {{"ITALIC", "StyledBlock"}};
    table["CommandBlock"]["LINK"] = {{"LINK", "Link"}};
    table["CommandBlock"]["IMAGE"] = {{"IMAGE", "Image"}};

    // Producciones para TableOrList
    table["TableOrList"]["BRACE_OPEN"] = {{"BRACE_OPEN", "Discriminator"}};

    // Producciones para Discriminator
    table["Discriminator"]["TABLE_KEYWORD"] = {{"TABLE_KEYWORD", "BRACE_CLOSE", "Table"}};
    table["Discriminator"]["LIST_KEYWORD"] = {{"LIST_KEYWORD", "BRACE_CLOSE", "List"}};

    // Producciones para Table
    table["Table"]["BRACE_OPEN"] = {{"TabularSpecifier", "RowList", "END", "BRACE_OPEN", "TABLE_KEYWORD", "BRACE_CLOSE"}};

    // Producciones para TabularSpecifier
    table["TabularSpecifier"]["BRACE_OPEN"] = {{"BRACE_OPEN", "ColumnSpecifier", "BRACE_CLOSE"}};

    // Producciones para ColumnSpecifier
    table["ColumnSpecifier"]["DIGITS"] = {{"DIGITS"}};

    // Producciones para RowList
    table["RowList"]["BEGIN"] = {{"ElementRow", "RowList"}};
    table["RowList"]["END"] = {{}};
    table["RowList"]["HEADER"] = {{"ElementRow", "RowList"}};
    table["RowList"]["BOLD"] = {{"ElementRow", "RowList"}};
    table["RowList"]["ITALIC"] = {{"ElementRow", "RowList"}};
    table["RowList"]["LINK"] = {{"ElementRow", "RowList"}};
    table["RowList"]["IMAGE"] = {{"ElementRow", "RowList"}};
    table["RowList"]["SPACE"] = {{"ElementRow", "RowList"}};
    table["RowList"]["NEWLINE"] = {{"ElementRow", "RowList"}};
    table["RowList"]["Text"] = {{"ElementRow", "RowList"}};

    // Producciones para ElementRow
    table["ElementRow"]["BEGIN"] = {{"Content", "ElementTail"}};
    table["ElementRow"]["HEADER"] = {{"Content", "ElementTail"}};
    table["ElementRow"]["BOLD"] = {{"Content", "ElementTail"}};
    table["ElementRow"]["ITALIC"] = {{"Content", "ElementTail"}};
    table["ElementRow"]["LINK"] = {{"Content", "ElementTail"}};
    table["ElementRow"]["IMAGE"] = {{"Content", "ElementTail"}};
    table["ElementRow"]["SPACE"] = {{"Content", "ElementTail"}};
    table["ElementRow"]["NEWLINE"] = {{"Content", "ElementTail"}};
    table["ElementRow"]["Text"] = {{"Content", "ElementTail"}};

    // Producciones para ElementTail
    table["ElementTail"]["CELL_SEPARATOR"] = {{"CELL_SEPARATOR", "Content", "ElementTail"}};
    table["ElementTail"]["NEWLINE"] = {{"NEWLINE"}};  // Para terminar la fila

    // Producciones para List
    table["List"]["END"] = {{"ItemList", "END", "BRACE_OPEN", "LIST_KEYWORD", "BRACE_CLOSE"}};
    table["List"]["CMD_ITEM"] = {{"ItemList", "END", "BRACE_OPEN", "LIST_KEYWORD", "BRACE_CLOSE"}};

    // Producciones para ItemList
    table["ItemList"]["END"] = {{}};
    table["ItemList"]["CMD_ITEM"] = {{"Item", "ItemList"}};

    // Producciones para Item
    table["Item"]["CMD_ITEM"] = {{"CMD_ITEM","SPACE", "Content", "NEWLINE"}};  // Para terminar el item

    // Producciones para Header
    table["Header"]["SQUARE_OPEN"] = {{"SQUARE_OPEN", "Text", "SQUARE_CLOSE", "BRACE_OPEN", "Content", "BRACE_CLOSE"}};

    // Producciones para StyledBlock
    table["StyledBlock"]["BRACE_OPEN"] = {{"BRACE_OPEN", "InnerContent", "BRACE_CLOSE"}};

    // Producciones para Link y Code
    table["Link"]["SQUARE_OPEN"] = {{"SQUARE_OPEN", "Text", "SQUARE_CLOSE", "BRACE_OPEN", "Text", "BRACE_CLOSE"}};
    table["Image"]["SQUARE_OPEN"] = {{"SQUARE_OPEN", "Text", "SQUARE_CLOSE", "BRACE_OPEN", "Text", "BRACE_CLOSE"}};

    // Producciones para InnerContent
    table["InnerContent"]["BEGIN"] = {{"CommandBlock", "InnerContent"}};
    table["InnerContent"]["HEADER"] = {{"CommandBlock", "InnerContent"}};
    table["InnerContent"]["BOLD"] = {{"CommandBlock", "InnerContent"}};
    table["InnerContent"]["ITALIC"] = {{"CommandBlock", "InnerContent"}};
    table["InnerContent"]["LINK"] = {{"CommandBlock", "InnerContent"}};
    table["InnerContent"]["IMAGE"] = {{"CommandBlock", "InnerContent"}};
    table["InnerContent"]["BRACE_CLOSE"] = {{}};
    table["InnerContent"]["SPACE"] = {{"SPACE", "InnerContent"}};
    table["InnerContent"]["NEWLINE"] = {{"NEWLINE", "InnerContent"}};
    table["InnerContent"]["Text"] = {{"Text", "InnerContent"}};

    // Producciones para Content
    table["Content"]["BEGIN"] = {{"CommandBlock"}};
    table["Content"]["HEADER"] = {{"CommandBlock"}};
    table["Content"]["BOLD"] = {{"CommandBlock"}};
    table["Content"]["ITALIC"] = {{"CommandBlock"}};
    table["Content"]["LINK"] = {{"CommandBlock"}};
    table["Content"]["IMAGE"] = {{"CommandBlock"}};
    table["Content"]["NEWLINE"] = {{"NEWLINE"}};
    table["Content"]["SPACE"] = {{"SPACE"}};
    table["Content"]["Text"] = {{"Text"}};
}

// Scanner
class Scanner {
private:
    std::string input;
    int position = 0;

public:
    Scanner(const std::string& text) : input(text) {}

    char peekchar() {
        return position < input.size() ? input[position] : EOF;
    }

    char getchar() {
        if (position < input.size()) {
            char currentChar = input[position];
            input.erase(position, 1);
            return currentChar;
        } else {
            return EOF;
        }
    }

    // Funcion para leer {
    void getBraceOpen() {
        if (peekchar() == '{') {
            getchar();
            tokens.push_back({"BRACE_OPEN", "{"});
        }
    }

    // Funcion para leer }
    void getBraceClose() {
        if (peekchar() == '}') {
            getchar();
            tokens.push_back({"BRACE_CLOSE", "}"});
        }
    }

    void getSquareClose() {
        if (peekchar() == ']') {
            getchar();
            tokens.push_back({"SQUARE_CLOSE", "]"});
        }
    }

    // Funcion para leer texto
    std::string getText(){
        std::string text;
        while (isalpha(peekchar()) || peekchar() == '.' || peekchar() == ',' || peekchar() == '!' || peekchar() == '?' || peekchar() == ':' || peekchar() == ';' || peekchar() == '*'
                || peekchar() == '-' || peekchar() == '_' || peekchar() == '(' || peekchar() == ')' || peekchar() == '+' || peekchar() == '=' || peekchar() == '<' || peekchar() == '>') {
            text += getchar();
        }
        return text;
    }

    // Funcion para obtener los comandos
    void getCommand() {
        if (peekchar() == '\\') {
            getchar();  // consume '\\'
            std::string command = getText();
            std::string tokenType;
            if (command == "begin") {
                tokenType = "BEGIN";
            } else if (command == "end") {
                tokenType = "END";
            } else if (command == "header") {
                tokenType = "HEADER";
            } else if (command == "item") {
                tokenType = "CMD_ITEM";
            } else if (command == "link") {
                tokenType = "LINK";
            } else if (command == "image") {
                tokenType = "IMAGE";
            } else if (command == "textbf") {
                tokenType = "BOLD";
            } else if (command == "textit") {
                tokenType = "ITALIC";
            } else {
                // Aquí se maneja cualquier comando no reconocido como texto.
                tokenType = "Text";
            }
            tokens.push_back({tokenType, command});
            if (peekchar() == '{') {
                getchar();
                tokens.push_back({"BRACE_OPEN", "{"});
                if (peekchar() == '\\') {
                    getCommand();
                } else { // For table and list
                    std::string ToL = getText();
                    if (ToL == "table") {
                        tokens.push_back({"TABLE_KEYWORD", ToL});
                        getBraceClose();
                        getBraceOpen();
                        std::string digits;
                        while (isdigit(peekchar())) {
                            digits += getchar();
                        }
                        if (!digits.empty()) {
                            tokens.push_back({"DIGITS", digits});
                        } else {
                            return;
                        }
                        getBraceClose();
                        while (peekchar() != '\\') {
                            getCommand();
                        }
                    } else if (ToL == "itemize") {
                        tokens.push_back({"LIST_KEYWORD", ToL});
                        getBraceClose();
                        std::string item = getText();
                        while (item == "\\item") {
                            getCommand();
                        }
                    } else {
                        tokens.push_back({"Text", ToL});
                        getBraceClose();
                    }
                }
            } else if (peekchar() == '[') {
                getchar();
                tokens.push_back({"SQUARE_OPEN", "["});
                std::string allText;
                while (peekchar() != ']') {
                    allText += getchar();
                }
                if (!allText.empty()) {
                    tokens.push_back({"Text", allText});
                }
                getSquareClose();
                if (peekchar() == '{'){
                    getchar();
                    tokens.push_back({"BRACE_OPEN", "{"});
                    std::string link;
                    while (peekchar() != '}') {
                        link += getchar();
                    }
                    if (!link.empty()) {
                        tokens.push_back({"Text", link});
                    }
                    getBraceClose();
                } else {
                    getBraceOpen();
                    getCommand();
                }
            } else {
                std::string text = getText();
                if (!text.empty()) {
                    tokens.push_back({"Text", text});
                }
                getBraceClose();
            }
        } else if ( peekchar() == '~') {
            getchar(); // Consume the "~"
            tokens.push_back({"NEWLINE", "~"});
        } else if ( peekchar() == ' ') {
            getchar(); // Consume the " "
            tokens.push_back({"SPACE", " "});
        } else if ( peekchar() == '&') {
            getchar(); // Consume the "&"
            tokens.push_back({"CELL_SEPARATOR", "&"});
        } else if ( peekchar() == '\0') {
            std::cout << "EOF" << std::endl;
            tokens.push_back({"EOF", ""});
        } else {
            std::string planeText = getText();
            if (!planeText.empty()) {
                tokens.push_back({"Text", planeText});
            }
            getBraceClose();
        }
    }


    std::vector<Token> tokenize() {
        while (position < input.size()) {
            //while (isspace(peekchar())) getchar();  // Skip whitespace but not newline
            getCommand(); // Get the command
        }
        return tokens;
    }
};

Token getNextToken() {
    static size_t currentIndex = 0;
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex++];
    } else {
        return {"EOF", ""}; // Retorna EOF cuando no hay mos tokens
    }
}

bool parse() {
    std::stack<std::string> parseStack;
    ParsingTable parsingTable;
    setupParsingTable(parsingTable);

    parseStack.push("EOF");  // Marca final de la pila
    parseStack.push("S");    // Símbolo inicial de la gramática

    Token currentToken = getNextToken();  // Primera palabra/token

    while (true) {
        std::string topOfStack = parseStack.top();

        if (topOfStack == "EOF" && currentToken.type == "EOF") {
            std::cout << "Parsing successful!" << std::endl;
            break;  // Éxito en el análisis
        } else if (topOfStack == currentToken.type) {
            parseStack.pop();  // Reconoce y elimina de la pila
            currentToken = getNextToken();  // Siguiente token
        } else {
            if (parsingTable[topOfStack].find(currentToken.type) != parsingTable[topOfStack].end()) {
                parseStack.pop();  // Elimina no terminal
                Production rule = parsingTable[topOfStack][currentToken.type];
                // Añade producciones a la pila en orden inverso
                for (auto it = rule.symbols.rbegin(); it != rule.symbols.rend(); ++it) {
                    parseStack.push(*it);
                }
            } else {
                // Error encontrado, aplicar manejo de errores de pánico
                std::cerr << "Error: Unexpected token " << currentToken.value << " of type " << currentToken.type << std::endl;
                success = false;

                // Manejo de errores de pánico: descarte tokens hasta encontrar un token de sincronización
                while (currentToken.type != "EOF" && currentToken.type != "BEGIN" &&
                       currentToken.type != "HEADER" && currentToken.type != "BOLD" &&
                       currentToken.type != "ITALIC" && currentToken.type != "LINK" &&
                       currentToken.type != "IMAGE" && currentToken.type != "END") {
                    currentToken = getNextToken();
                }

                // Descartar de la pila hasta encontrar un símbolo no terminal adecuado
                while (!parseStack.empty() && parsingTable.find(parseStack.top()) == parsingTable.end()) {
                    parseStack.pop();
                }

                // Si la pila está vacía, no se encontró un símbolo no terminal adecuado para la recuperación.
                if (parseStack.empty()) {
                    std::cerr << "Error: No suitable non-terminal found in parse stack for recovery." << std::endl;
                    break;
                }
            }
        }
    }
}

int main() {
    std::ifstream inputFile("../input.txt");
    std::ofstream outputFile("../output.md");
    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo de entrada.\n";
        return 1;
    }

    std::string line;
    std::string test_text;
    while (std::getline(inputFile, line)) {
        test_text += line;
    }

    Scanner scanner(test_text);
    std::vector<Token> tokens = scanner.tokenize();
    for (const auto& token : tokens) {
        std::cout << "<" << token.type << ", " << token.value << ">" << std::endl;
    }

    // Traduccion
    std::ifstream inputFile2("../input.txt");
    std::stringstream buffer;
    buffer << inputFile2.rdbuf();
    std::string text = buffer.str();

    std::string outputText = convertToMarkdown(text);
    outputFile << outputText;

    inputFile2.close();
    outputFile.close();

    // Parser
    parse();
    if (success) {
        std::cout << "Traduccion exitosa!" << std::endl;
    } else {
        std::cerr << "Error en la traduccion." << std::endl;
        //borramos el archivo de salida
        std::remove("../output.md");
    }


    return 0;
}