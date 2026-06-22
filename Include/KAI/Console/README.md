# Cross-platform Console

The Console provides Pi/Rho input, persistent history, automatic stack display,
shell integration, and networking on top of the Core Registry and Executor.

Prompts show only the active language symbol. The data stack is printed after
each command, top-first, with index `[0]` on the bottom line. Floating-point
values use the neutral/default value color.

The Console may host multiple live Executors. Inspection and debugging must
select an Executor explicitly by Registry handle. Each selected Executor is read
through its own `GetTree()` attachment; no global Tree is assumed. The private
NodeGLM snapshot is bounded to 1000 nodes and depth 32 per Executor.

Console startup initializes KAI `Logger`. Tree snapshot lifecycle, debugger
attachments/actions, and failures are logged through KAI. Machine-readable
snapshot records remain on stdout because they are protocol data.

