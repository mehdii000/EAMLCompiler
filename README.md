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

@define screen main:
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
@define greeting_card [bordered, rounded]:
    @text "Hello {name}!"
    @text "{message}"
    @button "Reply"

@use greeting_card with:
    name as "Alice"
    message as "Welcome to EAML!"

@use greeting_card with:
    name as "Bob"
    message as "Thanks for visiting!"
```

### Modifiers

```eaml
-- Boolean modifiers
[bordered]              -- Adds border
[rounded]               -- Rounded corners
[shadowed]              -- Drop shadow
[hidden]                -- Hidden by default

-- Property modifiers
[gap: 20px]             -- Spacing between items
[padding: 30px]         -- Internal spacing
[width: 300px]          -- Fixed width
[background: blue]      -- Background color
```

### Screens (Pages)

```eaml
@define screen home:
    @heading "Home Page"
    @text "Welcome!"

@define screen about [hidden]:
    @heading "About"
    @text "Learn more about us."
```

---

## 📖 Examples

### Personal Portfolio

```eaml
@title "My Portfolio"

@define project_card [bordered, rounded, shadowed]:
    @stack [padding: 20px, width: 300px]:
        @heading "{title}"
        @text "{description}"
        @button "View Project"

@define screen main:
    @center [padding: 50px]:
        @heading "Hi, I'm Mehdi"
        @text "Student & Developer"
        @button "View My Work"
    
    @heading "Projects"
    
    @row [gap: 20px, wrap]:
        @use project_card with:
            title as "Calculator"
            description as "A simple calculator app"
        
        @use project_card with:
            title as "Todo List"
            description as "Task manager"
        
        @use project_card with:
            title as "Weather App"
            description as "Real-time weather"
```

### Contact Form

```eaml
@define screen contact:
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
- [ ] Basic lexer and parser
- [ ] Component system
- [ ] Layout containers (row, stack, center)
- [ ] Modifier system
- [ ] Conditional rendering (`@if`)
- [ ] Loops (`@for`)
- [ ] Event handlers
- [ ] Animations
- [ ] CSS framework integration
- [ ] Live preview server
- [ ] VS Code syntax highlighting

## 📝 Language Specification

Full language specification will be available soon (i hope)

---

## 📄 License

This project is licensed under the GNU 3.0 License

---

<p align="center">Made with ❤️ for students learning web design</p>
