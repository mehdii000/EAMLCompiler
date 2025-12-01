#include "codegen.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <functional>
#include <fstream>

// -------------------------------
// Deep Clone Support
// -------------------------------
std::unique_ptr<ASTNode> CodeGenerator::cloneNode(const ASTNode* node) {
    if (!node) return nullptr;

    // Title
    if (auto* t = dynamic_cast<const TitleStmtNode*>(node)) {
        return std::make_unique<TitleStmtNode>(t->title);
    }

    // Text
    if (auto* t = dynamic_cast<const TextStmtNode*>(node)) {
        return std::make_unique<TextStmtNode>(t->text);
    }

    // Parameter
    if (auto* p = dynamic_cast<const ParameterNode*>(node)) {
        return std::make_unique<ParameterNode>(p->name, p->value);
    }

    // Load
    if (auto* l = dynamic_cast<const LoadStmtNode*>(node)) {
        auto out = std::make_unique<LoadStmtNode>(l->name);
        for (auto& param : l->parameters)
            out->parameters.push_back(
                std::unique_ptr<ParameterNode>(
                    static_cast<ParameterNode*>(cloneNode(param.get()).release())
                )
            );
        return out;
    }

    // Save
    if (auto* s = dynamic_cast<const SaveStmtNode*>(node)) {
        auto out = std::make_unique<SaveStmtNode>(s->name);
        for (auto& c : s->body)
            out->body.push_back(cloneNode(c.get()));
        return out;
    }

    // Screen
    if (auto* s = dynamic_cast<const ScreenStmtNode*>(node)) {
        auto out = std::make_unique<ScreenStmtNode>(s->name);
        for (auto& c : s->body)
            out->body.push_back(cloneNode(c.get()));
        return out;
    }

    throw std::runtime_error("Unknown AST node type in cloneNode()");
}

// -------------------------------
// Load Expander
// -------------------------------
void CodeGenerator::expandLoadsInList(std::vector<std::unique_ptr<ASTNode>>& list) {
    for (size_t i = 0; i < list.size(); /* increment handled manually */) {

        ASTNode* raw = list[i].get();

        // Case 1: @load — replace it
        if (auto* load = dynamic_cast<LoadStmtNode*>(raw)) {
            auto it = atSaveTable.find(load->name);

            if (it == atSaveTable.end()) {
                throw std::runtime_error("Undefined component: @load " + load->name);
            }

            // Remove the @load
            list.erase(list.begin() + i);

            // Insert clones of saved nodes
            const auto& saved = it->second;

            for (size_t k = 0; k < saved.size(); k++) {
                list.insert(list.begin() + i + k, cloneNode(saved[k].get()));
            }

            i += saved.size(); // Skip the newly inserted nodes
            continue;
        }

        // Case 2: A container statement — recurse into children
        if (auto* screen = dynamic_cast<ScreenStmtNode*>(raw)) {
            expandLoadsInList(screen->body);
        }
        else if (auto* save = dynamic_cast<SaveStmtNode*>(raw)) {
            expandLoadsInList(save->body);
        }

        // Default increment
        i++;
    }
}

// -------------------------------
// Main generate()
// -------------------------------
void CodeGenerator::generate(RootNode& root) {

    // 1. Collect all @save blocks (without modifying them)
    for (auto& stmt : root.statements) {
        if (auto* save = dynamic_cast<SaveStmtNode*>(stmt.get())) {
            // Deep-clone body to avoid ownership problems
            std::vector<std::unique_ptr<ASTNode>> cloned;

            for (auto& n : save->body)
                cloned.push_back(cloneNode(n.get()));

            atSaveTable[save->name] = std::move(cloned);
        }
    }

    // 2. Expand @load across *all* root statements
    expandLoadsInList(root.statements);

    // Create a file and stream the result of generateHTMLOutput on it
    std::ofstream outFile("output.html");
    if (outFile.is_open()) {
        outFile << generateHTMLOutput(root);
        outFile.close();
        std::cout << "HTML output generated successfully to output.html" << std::endl;
    } else {
        std::cerr << "Error: Unable to open output.html for writing." << std::endl;
    }


}

std::string CodeGenerator::generateHTMLOutput(RootNode& root) {
    std::ostringstream out;

    // Start HTML
    out << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<title>";

    // Find title node
    for (auto& stmt : root.statements) {
        if (auto* title = dynamic_cast<TitleStmtNode*>(stmt.get())) {
            out << title->title;
            break;
        }
    }

    out << "</title>\n</head>\n<body>\n";

    // Recursive helper lambda
    std::function<void(const ASTNode*, const std::unordered_map<std::string, std::string>&)> renderNode;
    renderNode = [&](const ASTNode* node, const std::unordered_map<std::string, std::string>& context) {
        if (!node) return;

        if (auto* text = dynamic_cast<const TextStmtNode*>(node)) {
            std::string txt = text->text;
            // Replace placeholders like {name} with context values
            for (auto& [k, v] : context) {
                size_t pos = 0;
                std::string ph = "{" + k + "}";
                while ((pos = txt.find(ph, pos)) != std::string::npos) {
                    txt.replace(pos, ph.length(), v);
                    pos += v.length();
                }
            }
            out << "<div>" << txt << "</div>\n";
        } 
        else if (auto* screen = dynamic_cast<const ScreenStmtNode*>(node)) {
            out << "<div class=\"screen\" id=\"" << screen->name << "\">\n";
            for (auto& stmt : screen->body)
                renderNode(stmt.get(), context);
            out << "</div>\n";
        } 
        else if (auto* load = dynamic_cast<const LoadStmtNode*>(node)) {
            // Build context from parameters
            std::unordered_map<std::string, std::string> paramContext;
            for (auto& p : load->parameters)
                paramContext[p->name] = p->value;

            // Lookup saved nodes
            auto it = atSaveTable.find(load->name);
            if (it != atSaveTable.end()) {
                for (auto& saved : it->second)
                    renderNode(saved.get(), paramContext);
            }
        } 
        else if (auto* save = dynamic_cast<const SaveStmtNode*>(node)) {
            // Usually saves are templates; don't render directly
        }
    };

    // Render all root statements except @save and @title
    for (auto& stmt : root.statements) {
        if (!dynamic_cast<SaveStmtNode*>(stmt.get()) && !dynamic_cast<TitleStmtNode*>(stmt.get()))
            renderNode(stmt.get(), {});
    }

    out << "</body>\n</html>\n";

    return out.str();
}
