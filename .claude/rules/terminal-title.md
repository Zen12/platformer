# Terminal Title Rule

## Update Terminal Title After Understanding Task

After understanding the user's task, update the terminal title to reflect what you're working on:

```bash
echo -ne "\033]0;Agent: short task\a"
```

**Requirements:**
- Keep the task description to 2-3 words maximum
- Replace "Agent" with your role (Researcher/Reviewer/Developer/Claude)
- Update immediately after understanding the user's request

**Examples:**
```bash
echo -ne "\033]0;Developer: FSM triggers\a"
echo -ne "\033]0;Researcher: animation bug\a"
echo -ne "\033]0;Reviewer: UI refactor\a"
```
