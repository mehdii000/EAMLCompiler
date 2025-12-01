# EAML - Extremely Accessible Markup Language

> A human-readable markup language that compiles to HTML/CSS, designed for teaching web design to beginners.

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

---

## 🎯 What is EAML?

EAML makes web design accessible. Less Verbose, Write simple, readable code and get subjectvly beautiful HTML/CSS pages.

**Before (HTML):**
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>My App</title>
</head>
<body>

<!-- main screen -->
<div class="main-screen">
    <div class="card">
        <div>hey Mehdi</div>
        <div class="buttons">
            <button class="button">Click 1</div>
            <button class="button">Click 2</div>
            <button class="button">Click 3</div>
        </div>
    </div>

    <p>Hello</p>
</div>

</body>
</html>

```

**After (EAML):**
```eaml
@title "My App"

@save card:
    @text "hey {name}"
    @load buttons

@save buttons:
    @button "Click 1"
    @button "Click 2"
    @button "Click 3"

@screen main:
    @load card with:
        name: "hey"
    @text "Mehdi"
```

---

## ✨ Features

- **♻️ Reusable Components** - Define once, use everywhere with parameters
- **🚀 Simple Syntax** - No angle brackets, no closing tags, no confusion
- **🎨 Layouts Made Easy** - Row, stack, center - that's all you need
---

### Installation (WIP)

```bash
# Clone the repository
git clone https://github.com/mehdii000/EAMLCompiler.git
cd EAMLCompiler

# Build
mkdir build && cd build
cmake ..
make

# Run
./eaml ../examples/hello.eaml
```

### Your First EAML Program

Create `hello.eaml`:
```eaml
@title "My First Page"

@screen main:
    @heading "Hello, EAML!"
    @text "This is my first webpage."
    @button "Click Me"
```

Compile it:
```bash
./eaml hello.eaml
```

Open `output.html` in your browser! 🎉

---

## 📚 Language Overview

### Basic Elements

```eaml
@text "Plain text content"
@heading "Page Title"
@button "Click Me"
@image "name" from "photo.jpg"
@link "Google" to "https://google.com"
```

### Layouts

```eaml
-- Horizontal layout
@row [gap: 10px]:
    @button "One"
    @button "Two"
    @button "Three"

-- Vertical layout
@stack [gap: 20px]:
    @text "First item"
    @text "Second item"
    @text "Third item"
```

### Components with Parameters

```eaml
@save greeting_card [bordered, rounded]:
    @text "Hello {name}!"
    @text "{message}"
    @button "Reply"

@load greeting_card with:
    name: "Alice"
    message: "Welcome to EAML!"

@load greeting_card with:
    name: "Bob"
    message: "Thanks for visiting!"
```

### Modifiers

```eaml
-- Boolean modifiers
[bordered]              -- Adds border
[rounded]               -- Rounded corners
[shadowed]              -- Drop shadow
[hidden]                -- Hidden by default
...

-- Property modifiers
[gap: 20px]             -- Spacing between items
[padding: 30px]         -- Internal spacing
[width: 300px]          -- Fixed width
[background: blue]      -- Background color
```

### Screens (Pages)

```eaml
@screen home:
    @heading "Home Page"
    @text "Welcome!"

@screen about [hidden]:
    @heading "About"
    @text "Learn more about us."
```

---

## 📖 Examples

### Personal Portfolio

```eaml
@title "My Portfolio"

@save project_card [bordered, rounded, shadowed]:
    @stack [padding: 20px, width: 300px]:
        @heading "{title}"
        @text "{description}"
        @button "View Project"

@screen main:
    @center [padding: 50px]:
        @heading "Hi, I'm Mehdi"
        @text "Student & Developer"
        @button "View My Work"
    
    @heading "Projects"
    
    @row [gap: 20px, wrap]:
        @use project_card with:
            title: "Calculator"
            description: "A simple calculator app"
        
        @load project_card with:
            title: "Todo List"
            description: "Task manager"
        
        @load project_card with:
            title: "Weather App"
            description: "Real-time weather"
```

### Contact Form

```eaml
@screen contact:
    @center:
        @stack [width: 400px, bordered, padding: 30px]:
            @heading "Get In Touch"
            
            @input [type: text, placeholder: "Your Name"]
            @input [type: email, placeholder: "Your Email"]
            @input [type: textarea, placeholder: "Message"]
            
            @button "Send Message" [background: blue]
```

## 🗺️ Roadmap

- [x] Think of the project and think about starting it
- [x] Basic lexer and parser
- [x] Component system
- [ ] Layout containers (row, stack, center)
- [ ] Modifier system
- [ ] Loops (`@for`)
- [ ] CSS framework integration
- [ ] Live preview server
- [ ] VS Code syntax highlighting

## 📝 Language Specification

Full language specification will be available soon (i hope)

---

## 📄 License

This project is licensed under the GNU 3.0 License

---

<p align="center">Made with ❤️ for every curious mind across the globe.</p>
