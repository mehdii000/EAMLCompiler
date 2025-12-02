## About

EAML is a lightweight, beginner-friendly markup language that simplifies web design. It allows you to create reusable components and clean layouts with minimal syntax, compiling directly to HTML/CSS.

## EAML - Eaml's Average Markdown Language

EAML makes web design accessible. Write less verbose, resuable, readable code and get subjectvly the same HTML/CSS results.

**Before (HTML):**
```html
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Simple Title</title>
</head>
<body>
    <div class="screen" id="main">
        <h1>Hey there!</h1>
        <button>click me</button>
        <br>
        <div>hey i am Alice</div>
        <div>i am 12 old</div>
        <br>
        <div>hey i am Mehdi</div>
        <div>i am 13 old</div>
        <br>
        <div>hey i am Eve</div>
        <div>i am 16 old</div>
        <br>
    </div>
</body>
</html>
```

**After (EAML):**
```eaml
@title "Simple Title"

@save card:
    @text "hey i am {name}"
    @text "i am {age} old"
    @br

@screen main:
    @h1 "Hey there!"
    @button "click me"

    @br

    @load card with:
        name: "Alice"
        age: 12
    @load card with:
        name: "Mehdi"
        age: 13
    @load card with:
        name: "Eve"
        age: 16
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
- [x] Component system (@load & @save)
- [x] @load with parameters (Ex. name: "Adam")
- [x] Generic html components as @component "value"

- [ ] Layout containers (row, stack, center)
- [ ] Pre-definied generic styling to order everything around

- [ ] Modifier system (Ex. [rounded, shadowed])
- [ ] Basic inline css

- [ ] VS Code syntax highlighting

## 📝 Language Specification

Full language specification will be available soon (i hope)

---

## 📄 License

This project is licensed under the GNU 3.0 License

---

<p align="center">Made with ❤️ for every curious mind across the globe.</p>
