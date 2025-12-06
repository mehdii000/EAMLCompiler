#include "codegen.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <functional>
#include <fstream>
#include "codeutils.hpp"

void replaceNodeValueWithAppropriateContext(ASTNode* node, const std::unordered_map<std::string, std::string>& context) {
    if (!node) return;

    if (auto* param = dynamic_cast<TextStmtNode*>(node)) {
        std::string txt = param->text;
        // Replace placeholders like {name} with context values
        for (auto& [k, v] : context) {
            size_t pos = 0;
            std::string ph = "{" + k + "}";
            while ((pos = txt.find(ph, pos)) != std::string::npos) {
                txt.replace(pos, ph.length(), v);
                pos += v.length();
            }
        }
        param->text = txt;
    } else if (auto* generic = dynamic_cast<GenericAtStmtNode*>(node)) {
        // Replace generic->value {param} with its value
        std::string txt = generic->value;
        for (auto& [k, v] : context) {
            size_t pos = 0;
            std::string ph = "{" + k + "}";
            while ((pos = txt.find(ph, pos)) != std::string::npos) {
                txt.replace(pos, ph.length(), v);
                pos += v.length();
            }
        }
        generic->value = txt;
    }


    if (node->children()) {
        for (auto& child : *node->children()) {
            replaceNodeValueWithAppropriateContext(child.get(), context);
        }
    }
}


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

    // GenericAt
    if (auto* g = dynamic_cast<const GenericAtStmtNode*>(node)) {
        return std::make_unique<GenericAtStmtNode>(g->name, g->value);
    }

    // Layout
    if (auto* l = dynamic_cast<const LayoutStmtNode*>(node)) {
        auto out = std::make_unique<LayoutStmtNode>();
        out->bordered = true;
        out->layout = l->layout;
        for (auto& c : l->body)
            out->body.push_back(cloneNode(c.get()));
        return out;
    }
    

    throw std::runtime_error("Unknown AST node type in cloneNode()");
}

// -------------------------------
// Load Expander
// -------------------------------
void CodeGenerator::expandLoadsInList(std::vector<std::unique_ptr<ASTNode>>& list) {
    for (size_t i = 0; i < list.size(); /* manual increment */) {

        ASTNode* raw = list[i].get();

        // ===========================
        // CASE 1: @load
        // ===========================
        if (auto* load = dynamic_cast<LoadStmtNode*>(raw)) {

            // First expand inside the load node itself if it has childrens
            if (load->children() && !load->children()->empty()) {
                expandLoadsInList(*load->children());
            }

            // Now find the saved template
            auto it = atSaveTable.find(load->name);
            if (it == atSaveTable.end()) {
                throw std::runtime_error("Undefined component: @load " + load->name);
            }

            const auto& savedTemplate = it->second;

            // Build parameter context
            std::unordered_map<std::string, std::string> paramContext;
            for (auto& param : load->parameters)
                paramContext[param->name] = param->value;

            // ---- Remove the load node ----
            list.erase(list.begin() + i);

            size_t insertCount = 0;

            // Clone + apply params
            for (const auto& tpl : savedTemplate) {

                std::unique_ptr<ASTNode> cloned = cloneNode(tpl.get());

                if (!paramContext.empty())
                    replaceNodeValueWithAppropriateContext(cloned.get(), paramContext);

                list.insert(list.begin() + i + insertCount, std::move(cloned));
                insertCount++;
            }

            i += insertCount;
            continue;
        }

        // ===========================
        // CASE 2: Containers
        // ===========================
        if (auto* screen = dynamic_cast<ScreenStmtNode*>(raw)) {
            expandLoadsInList(screen->body);
        }
        else if (auto* save = dynamic_cast<SaveStmtNode*>(raw)) {
            expandLoadsInList(save->body);
        }

        i++; // default
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

    out << "</title>\n</head>";

    // Prototyping css
    out << "<style>\n";
    out << readFile("style.css"); 
    out << "</style>\n";

    out << "<body>\n";

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
            out << "<p>" << txt << "</p>\n";
        }
        else if (auto* generic = dynamic_cast<const GenericAtStmtNode*>(node)) {
            std::string html_header = generic->name;
            std::string txt = generic->value;
            
            // Replace placeholders like {name} with context values
            for (auto& [k, v] : context) {
                size_t pos = 0;
                std::string ph = "{" + k + "}";
                while ((pos = txt.find(ph, pos)) != std::string::npos) {
                    txt.replace(pos, ph.length(), v);
                    pos += v.length();
                }
            }

            for (auto& [k, v] : context) {
                html_header += " " + k + "=\"" + v + "\"";
            }
            out << "<" << html_header << ">\n";
            out << txt;
            out << "</" << html_header << ">\n";
        }
        else if (auto* screen = dynamic_cast<const ScreenStmtNode*>(node)) {
            out << "<div class=\"screen\" id=\"" << screen->name << "\">\n";
            for (auto& stmt : screen->body)
                renderNode(stmt.get(), context);
            out << "</div>\n";
        } else if (auto* layout = dynamic_cast<const LayoutStmtNode*>(node)) {

            if (layout->bordered == true) {
                out << "<div class=\"layout main-borders\" id=\"" << layout->layout << "\">\n";
            } else {
                out << "<div class=\"layout\" id=\"" << layout->layout << "\">\n";
            }


            for (auto& stmt : layout->body)
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
