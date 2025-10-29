# 🎯 TARGET SHOOTER GAME

A simple arcade shooting game built with C++ and graphics.h library demonstrating Object-Oriented Programming concepts.

---

## 📝 About

This is a mini-project for OOPCG (Object-Oriented Programming and Computer Graphics) course. Control a gun turret to shoot moving targets while avoiding dangerous bombs!

---

## ✨ Features

- 🎯 Multiple target types (Regular, Fast, Bonus, Bomb)
- 💥 Animated explosions
- 🎨 Colorful graphics
- ❤️ Lives system (3 lives)
- 💾 High score saving
- 📈 Progressive difficulty

---

## 🎮 Game Elements

### Targets

| Type | Points | Special |
|------|--------|---------|
| 🔴 Regular | +10 | Basic targets |
| 🔴💨 Fast | +20 | Moves faster |
| 🟢 Bonus | +50 | Gives extra life |
| 💣 Bomb | -30 | **AVOID! Lose life** |

---

## 🔧 Installation

### Requirements
- Linux (Ubuntu/Debian)
- g++ compiler
- libgraph library

### Install
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential libgraph-dev

# Compile
g++ gun.cpp -lgraph -o game

# Run
./game
```

---

## 🕹️ Controls

| Key | Action |
|-----|--------|
| **A** or **←** | Move left |
| **D** or **→** | Move right |
| **Space** | Shoot |
| **P** | Pause |
| **Q** / **ESC** | Quit |

---

## 🎯 How to Play

1. **Shoot colorful targets** to score points
2. **Avoid red bombs** - they hurt you!
3. **Hit green bonus targets** for extra lives
4. **Clear all targets** to advance to next level
5. **Game ends** when lives reach 0

### Tips
- Aim ahead of fast-moving targets
- Always shoot bonus targets for extra lives
- Never shoot bombs!
- You get 20 bullets + 15 more each level

---

## 🧩 OOP Concepts Used

### 1. Abstraction
- Abstract `GameObject` base class with pure virtual functions

### 2. Inheritance
```
GameObject
  ├── Gun
  ├── Bullet
  ├── Target
  │   ├── FastTarget
  │   ├── BonusTarget
  │   └── BombTarget
  └── Explosion
```

### 3. Polymorphism
- Virtual `draw()` and `update()` methods
- Different behaviors for each target type

### 4. Encapsulation
- Private members with public methods
- Data hiding

### 5. File Handling
- High score saved to `highscore.dat`

### 6. Dynamic Memory
- Dynamic object creation and deletion

---

## 📊 Scoring

- Regular Target: **+10 points**
- Fast Target: **+20 points**
- Bonus Target: **+50 points + Life**
- Bomb Target: **-30 points + Lose Life**

---

## 📁 Files

```
gun.cpp           - Main game code
highscore.dat     - Saved high score
README.md         - This file
```

---

## 🚀 Game Features

### Visual Effects
- Solid 3D sphere targets
- Glowing explosions
- Speed lines on fast targets
- Pulsing bomb animation

### Gameplay
- Lives: Start with 3, max 5
- Bullets: 20 per level + 15 bonus
- Speed: Targets move 1.3x faster
- Levels: Unlimited progression

---

## 🎓 Academic Info

**Project Type:** Mini Project  
**Subject:** OOPCG (Object-Oriented Programming & Computer Graphics)  
**Language:** C++  
**Graphics:** libgraph (BGI compatible)

---

## 👥 Developers

| Name | Roll No | Contribution |
|------|---------|--------------|
| [Kunal Sable] | [Roll No 53] | Game Logic & OOP Design |
| [Rehan Shaikh] | [Roll No 55] | Graphics & Visual Effects |
| [Rukar Shinde] | [Roll No 57] | Target System & Collision |
| [Pragati suryawanshi] | [Roll No 58] | File Handling & Testing |

**Class:** [S.Y-B]  
**College:** [K.K.W.I.E.E.R]  
**Guide:** [Prof.Priti Vaidya]

---

## 📄 License

Free to use for educational purposes.

---

## 🐛 Troubleshooting

**Problem:** Arrow keys not working  
**Solution:** Use A/D keys instead

**Problem:** Graphics not initializing  
**Solution:** Install libgraph-dev package

**Problem:** Compilation errors  
**Solution:** Make sure you have g++ installed

---

## 💡 Quick Start

```bash
# One-line setup and run
sudo apt-get install libgraph-dev && g++ gun.cpp -lgraph -o game && ./game
```

---

**Enjoy the game! 🎮**
