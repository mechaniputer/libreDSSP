// Note: Unlike most files in the libreDSSP project, this one was produced with AI assistance.
// Any libreDSSP files without this disclaimer do not contain AI code or text.

document.addEventListener("DOMContentLoaded", function() {
  if (typeof hljs !== 'undefined') {
    
    hljs.registerLanguage('dssp', function(hljs) {
      return {
        aliases: ['dssp'],
        keywords: {
          // Primary control-flow structural keywords
          keyword: 'IF- IF0 IF+ BR- BR0 BR+ BRS BR ELSE RP DO BYE',
        },
        contains: [
          // 1. Bracket Comments: [ like this ]
          hljs.COMMENT('\\[', '\\]'),
          
          // 2. ERROR Printouts: Captures "ERR: ..." lines safely
          {
            className: 'deletion', 
            begin: '\\bERR:\\s*.*$',
          },
          
          // 3. Multi-line BR Block Matcher (High Precedence)
          {
            begin: '\\bBR\\b',
            end: '(?=;|$)',
            keywords: {
              keyword: 'BR ELSE',
            },
            contains: [
              hljs.COMMENT('\\[', '\\]'),
              hljs.C_NUMBER_MODE,
              {
                className: 'string',
                begin: '\\."', end: '"'
              }
            ]
          },
          
          // 4. Print statements: ."Hello world"
          {
            className: 'string',
            begin: '\\."', end: '"',
            contains: [{ begin: '\\\\.' }]
          },
          
          // 5. Standard Strings
          hljs.QUOTE_STRING_MODE,
          
          // 6. Word Declarations: Matches word name after an isolated colon
          {
            className: 'title.function',
            begin: '(?<=(^|\\s):)\\s+\\S+'
          },

          // 7. Variable Declarations: Matches the word right after "VAR "
          {
            className: 'variable',
            begin: '(?<=\\bVAR\\s+)\\s*\\S+'
          },

          // 8. Dictionary Names: Matches tokens beginning with $
          {
            className: 'symbol',
            begin: '\\$\\S+'
          },
          
          // 9. Core Words List (Subtle Bolding via built_in)
          // Escapes math characters so they don't break regex mapping trees
          // Does not include pseudo-literals 0, 1, 2, 4, 8.
          {
            className: 'built_in',
            begin: '\\b(\\+|\\-|\\*|\\/|<|>|=|\\.|\\.\\.|!|\\?\\$|1\\+|1\\-|2\\+|2\\-|3\\+|3\\-|4\\+|4\\-|ABS|B10|CT|CR|D|DEEP|DS|E2|E3|E4|ET|EX|EX\\+|EX0|EX\\-|EXT|GROW|NEG|NOP|TIN|TON|TOS|USE|GROW|SHUT|C|C2|C3|C4)\\b'
          },
          
          // 10. Word Markers: Only highlight : and ; when they are standalone structural tokens
          {
            className: 'symbol',
            begin: '(^|\\s)(:|;)(\\s|$)'
          },
          
          // 11. Catch-all for standard integers outside the core words pattern
          hljs.C_NUMBER_MODE
        ]
      };
    });

    // 2. Locate all code blocks and force plain or untagged blocks to use DSSP
    document.querySelectorAll('pre code').forEach((block) => {
      const hasLanguage = Array.from(block.classList).some(className => 
        className.startsWith('language-') && className !== 'language-none' && className !== 'language-text'
      );
      
      if (!hasLanguage) {
        block.classList.add('language-dssp');
      }
    });

    // 3. Kick off global highlighting engine
    hljs.highlightAll();
  }
});
