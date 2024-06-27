//
// Created by marck on 21/06/2024.
//

#ifndef LATEXC_TRADUCCION_H
#define LATEXC_TRADUCCION_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <stack>
#include <regex>

// Función para procesar contenido anidado en negrita y cursiva
std::string processStyledContent(const std::string &content) {
    std::string processedContent = content;
    std::regex boldItalicRegex(R"(\\textbf\{([^{}]*\\textit\{[^{}]*\}[^{}]*)\})");
    std::regex italicBoldRegex(R"(\\textit\{([^{}]*\\textbf\{[^{}]*\}[^{}]*)\})");
    std::regex boldRegex(R"(\\textbf\{([^{}]*)\})");
    std::regex italicRegex(R"(\\textit\{([^{}]*)\})");

    // Procesar negritas con contenido en cursiva anidado
    processedContent = std::regex_replace(processedContent, boldItalicRegex, "**$1**");
    processedContent = std::regex_replace(processedContent, italicRegex, "*$1*");

    // Procesar cursivas con contenido en negrita anidado
    processedContent = std::regex_replace(processedContent, italicBoldRegex, "*$1*");
    processedContent = std::regex_replace(processedContent, boldRegex, "**$1**");

    return processedContent;
}

std::string processLine(std::string line) {
    // Expresiones regulares para detectar patrones
    std::regex headerRegex(R"(\\header\[(\d+)\]\{(.*?)\}~)");
    std::regex boldRegex(R"(\\textbf\{([^{}]*)\})");
    std::regex italicRegex(R"(\\textit\{([^{}]*)\})");
    std::regex linkRegex(R"(\\link\[(.*?)\]\{(.*?)\}~)");
    std::regex imageRegex(R"(\\image\[(.*?)\]\{(.*?)\}~)");
    std::regex itemRegex(R"(\\item\s+(.*?)~)");

    std::smatch match;

    // Procesamiento de encabezados
    if (std::regex_search(line, match, headerRegex)) {
        int level = std::stoi(match[1].str());
        line = std::string(level, '#') + " " + processStyledContent("**" + match[2].str() + "**");
    } else {
        // Procesar negritas e itálicas anidadas
        line = processStyledContent(line);

        // Procesamiento de enlaces
        line = std::regex_replace(line, linkRegex, "[$2]($1)");

        // Procesamiento de código
        line = std::regex_replace(line, imageRegex, "![$1]($2)");

        // Procesamiento de elementos de lista
        line = std::regex_replace(line, itemRegex, "- $1");
    }

    // Eliminar el carácter '~' al final de las líneas
    if (!line.empty() && line.back() == '~') {
        line.pop_back();
    }

    return line;
}

std::string convertToMarkdown(const std::string &input) {
    std::stringstream ss(input);
    std::string line;
    std::string output;
    std::regex tableStartRegex(R"(\\begin\{table\}\{(\d+)\})");
    std::regex tableEndRegex(R"(\\end\{table\}~)");
    std::regex listStartRegex(R"(\\begin\{itemize\})");
    std::regex listEndRegex(R"(\\end\{itemize\}~)");
    std::smatch match;

    bool inTable = false;
    bool inList = false;

    while (std::getline(ss, line)) {
        if (std::regex_search(line, match, tableStartRegex)) {
            inTable = true;
            output.append("\n");
        } else if (std::regex_search(line, match, tableEndRegex)) {
            inTable = false;
            output.append("\n");
        } else if (std::regex_search(line, match, listStartRegex)) {
            inList = true;
            output.append("\n");
        } else if (std::regex_search(line, match, listEndRegex)) {
            inList = false;
            output.append("\n");
        } else {
            if (inTable) {
                // Procesar celdas de la tabla
                line = processStyledContent(line);
                line = std::regex_replace(line, std::regex(R"(\&)"), " | ");
                if (!line.empty() && line.back() == '~') {
                    line.pop_back();
                }
                output.append("| " + line + " |\n");
            } else {
                if (inList) {
                    line = processLine(line);
                    if (!line.empty() && line.back() == '~') {
                        line.pop_back();
                    }
                    output.append(line + "\n");
                } else {
                    output.append(processLine(line) + "\n");
                }
            }
        }
    }

    return output;
}

#endif //LATEXC_TRADUCCION_H
