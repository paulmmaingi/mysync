# Notes

```math
\begin{align*}
  \text{Modifications Needed} &\leq \sum_{i} F_i \times \left(F_{\text{num}} - 1\right)\\
  &\leq \left(F_{\text{num}} - 1\right) \times \sum_{i} F_i\\
\end{align*}
```

where: $F_i$ is the number of files in the $i$-th directory and $F_{\text{num}}$ is the number of total directories; $i = 1, 2, \ldots, F_{\text{num}}$.
From the second equation, it's $\leq$ total number of files in all directories multiplied by the number of directories minus one.

A bit inefficient; adds unnecessary modifications to the modification list but still ends up with correct implementation I suppose. Consider: $\texttt{dir1/file1}$, $\texttt{dir2/file1}$ and $\texttt{dir3/file1}$ where the recency of the files is $\texttt{dir1/file1} \geq \texttt{dir2/file1} \geq \texttt{dir3/file1}$ (where $A \geq B$ means $A$ is more recent than $B$). The modification list would make the following comparisons:\
$\texttt{dir1/file1}$ vs $\texttt{dir2/file1}$ $\rightarrow$ $\texttt{dir1/file1}$\
$\texttt{dir1/file1}$ vs $\texttt{dir3/file1}$ $\rightarrow$ $\texttt{dir1/file1}$\
$\texttt{dir2/file1}$ vs $\texttt{dir3/file1}$ $\rightarrow$ $\texttt{dir2/file1}$\
The losing file in these comparisons will get modified and thus contain the most recent data. Regardless of the order of the directories or the comparison order above, the most recent file ends up winning as all the files that lose to a comparison with it will get updated and thus contain the most recent data. For example, in the above example, $\texttt{dir2/file1}$ will get updated first to have the most recent data from $\texttt{dir1/file1}$ and then $\texttt{dir3/file1}$ will get updated to have the most recent data from $\texttt{dir1/file1}$. The last comparison is unnecessary in this case but still ends up with the correct implementation as $\texttt{dir3/file1}$ and $\texttt{dir2/file1}$ both contain the most recent data from $\texttt{dir1/file1}$. If the order of comparisons were different, say the last comparison was made first, the same result would be obtained as the last 2 would ensure the most recent data is in all files.

Thought of a better way to word this: every directory gets compared to every other directory and so the directory with the most recent data will always update the other directories in its comparison with them and they can never get a worse mtime if already they have the most recent data.

## Line count

```bash
git ls-files | xargs wc -l

   28 .clang-format
   40 .github/workflows/c-cpp.yml
    4 .gitignore
    2 .vscode/settings.json
   21 LICENSE
   45 Makefile
   74 README.md
   21 notes.md
    0 obj/.gitkeep
  458 src/data_structs.c
   61 src/extra_functions.c
   45 src/headers/base.h
  328 src/headers/data_structs.h
   14 src/headers/mysync.h
   40 src/headers/read_dir.h
   54 src/headers/sync.h
  106 src/mysync.c
  153 src/read_dir.c
  271 src/sync.c
 1765 total
```
