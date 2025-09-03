# ‎‧₊˚✧[TaskZero]✧˚₊‧

TaskZero is a to-do-list app in terminal, created by a guy who love using terminal.

## ✨Features

### ⚙️ Base features

 - 💡 Arguments:

```bash
[arguments]: [name] [deadline] [consequence]
# name limit : 20 chars
# deadline   : DD/MM/YY
# consequence: 0 -> 10
```

 - 🚩 Flags:

```bash
[flags]: -n, -d, -c
# -n: exclude name (use for update if you don't want to change name)
# -d: set deadline    = NaH
# -c: set consequence = 0
```

 - 📟 Commands:

```bash
taskzero
  > add    [flags]  [arguments]
  > update [flags]  [task-id] [arguments]
  > delete          [task-id]
  > mark-done       [task-id]
  > mark-in-progess [task-id]
```
