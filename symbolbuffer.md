# Symbolbuffer

Der `Symbolbuffer` kapselt die Konvertierung zwischen normalem C-String und dem 5-Bit-Symbolformat des TTYs. In diesem Projekt arbeitet er auf festen RAM-Puffern und nicht mehr über Heap-Allokation.

## Klassendiagramm

```mermaid
classDiagram
    class SymbolBuffer {
        +sbf_len(sbf_t sbf) uint32_t
        +sbf_appendSym(sbf_t head, symbol_t sym) sbf_t
        +sbf_concaternate(sbf_t head, sbf_t tail, uint8_t keepTail) sbf_t
        +sbf_convertToChar(symbol_t symbol, char* target, char* _newLine, tty_mode_t* current_mode, uint32_t* carriage_pos, uint32_t* last_lf) symbol_t
        +sbf_charToSymbolBuffer(sbf_t _out, char _c, tty_mode_t* _currentMode) sbf_t
        +sbf_convertToString(sbf_t _inSbf, char* _outStr, uint16_t outSize, char* _newLine) char*
        +sbf_convertToSymbolBuffer(char* _inStr, sbf_t _out) sbf_t
    }

    class tty_types.h {
        <<types>>
        symbol_t
        sbf_t
        tty_mode_t
        SBF_TERMINATOR
        STR_MAIN_SIZE
        SBF_MAIN_SIZE
    }

    SymbolBuffer ..> tty_types.h : uses types
```

## Flowchart

```mermaid
flowchart TD
    A[Input char* string] --> B[sbf_convertToSymbolBuffer]
    B --> C[Reset output buffer]
    C --> D[Read next character]
    D --> E{Character is CR/LF?}
    E -- yes --> F[Map to cr / lf]
    E -- no --> G{TTY mode figures?}
    G -- yes --> H[Use char_to_symFIGS]
    G -- no --> I[Use char_to_symLTRS]
    I --> J{Fallback needed?}
    J -- yes --> K[Use char_to_symCommon]
    J -- no --> L[Append symbol]
    H --> L
    F --> L
    L --> M{More characters?}
    M -- yes --> D
    M -- no --> N[Write SBF_TERMINATOR]
    N --> O[sbf_t buffer]

    O --> P[sbf_convertToString]
    P --> Q[Read symbol by symbol]
    Q --> R{sbf_convertToChar}
    R --> S[Convert symbol to char]
    S --> T[Write into output string]
    T --> U{More symbols?}
    U -- yes --> Q
    U -- no --> V[Terminate with '\0']
```

## Kurzbeschreibung

- `sbf_len` sucht den Terminator im Symbolbuffer.
- `sbf_appendSym` hängt ein Symbol an, solange der feste Puffer noch Platz hat.
- `sbf_concaternate` kopiert einen zweiten Buffer an den ersten Buffer an.
- `sbf_convertToChar` wandelt ein Symbol zurück in ein Zeichen.
- `sbf_charToSymbolBuffer` wandelt ein einzelnes Zeichen in ein Symbol um.
- `sbf_convertToString` erzeugt aus einem Symbolbuffer einen C-String.
- `sbf_convertToSymbolBuffer` erzeugt aus einem C-String einen Symbolbuffer.

## Verwendete Puffer

- `sbf_main[300]` ist der feste Symbolbuffer.
- `str_main[256]` ist der feste Stringbuffer.
- Beide liegen in `tty.c` und werden von der TTY-Schicht gemeinsam benutzt.
