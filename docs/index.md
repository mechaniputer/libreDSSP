# Welcome to libreDSSP Docs

Welcome to the official documentation for the libreDSSP programming system.

libreDSSP (libre Dialog System for Structured Programming) is a free software interpreter for the DSSP language. This language originated in the Soviet Union and was inspired by the architecture of the Setun ternary computer. It is very similar to Forth but has more compact syntax along with arguably more consistent rules regarding the behavior of stack operations and the evaluation of variables and addresses. DSSP also supports top-down programming, meaning that as-yet undefined words can be referenced in other words.

These docs cover the usage of libreDSSP, as well as some of the internals to make it easier to modify or extend libreDSSP.
For information about building, installing, or contributing code to libreDSSP, please visit our [GitHub repository](https://github.com/mechaniputer/libreDSSP)

## Documentation Status
Most of what currently works is documented, but this is a moving target.
A new section is being added to cover libreDSSP internals.

## Licensing
All libreDSSP code, all libreDSSP example programs, all libreDSSP markdown documentation, the vim source highlighting configuration files, and all custom files that configure and format the documentation, are licensed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html).

[Properdocs](https://properdocs.org/) (the tool we use to prepare our documentation) is under the [MkDocs License (BSD)](https://properdocs.org/about/license/).


## AI Policy
One of the goals of this project is to develop a full understanding of DSSP including implementation pitfalls.
A second goal is to produce something maintainable and lasting. Lastly, I want to have fun doing it.
As a consequence of these goals, this is a human-written project.

None of the documentation was AI-generated, and **no significant code in libreDSSP itself is AI-generated**.
At present, the only really AI-generated code in libreDSSP is found in:

- Parts of [dssp.js](https://github.com/mechaniputer/libreDSSP/blob/master/docs/dssp.js), which handles DSSP source highlighting on the official docs (obviously highlight.js doesn't know about DSSP).
- Parts of the [configure](https://github.com/mechaniputer/libreDSSP/blob/master/configure) script.
- Parts of the [Makefile](https://github.com/mechaniputer/libreDSSP/blob/master/Makefile).

Even these files were generated with many rounds of human input and substantial manual revision, making them eligible for copyright.

Aside from the above, I have used AI assistance when working on libreDSSP for the following tasks:

- Taking inventory of the project state when I resumed work after a multi-year delay and had forgotten what I was doing nearly 7 years prior.
- Translation of original Russian DSSP documents and papers to help me learn DSSP syntax.
- Planning the addition of new features and to reduce the risk of forgetting to apply changes to older code.
- Occasionally, AI auto-completion was used during repetitive or tedious refactoring.

In all of the above, all AI output was taken with more than a few grains of salt and everything was also rechecked in other ways. The AI tools were just one of many data points so that they added to, rather than harmed, the likelihood of correctness.