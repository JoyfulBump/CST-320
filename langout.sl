.function main
main:
    ADJSP 8
    PUSH 10
    DUP
    PUSH 0
    PUSHFP
    PLUS
    POPVARIND
    POP
    PUSH 20
    DUP
    PUSH 0
    PUSHFP
    PLUS
    POPVARIND
    POP
    PUSHVAR 0
    CALL @print
    POP
    PUSHVAR 0
    CALL @print
    POP
    PUSH 0
    RETURNV
