# Sejong Univ. CPU

## Introduce

Implement Sejong University CPU with C++.
And I also implemented the sj-assembler and sj-debugger to make it easy to use.

Special thanks
* [Professor Ki-Woong Park](http://home.sejong.ac.kr/~woongbak/)

## Instructions

| Maching | Language Instruction |
|:--------:|:---------:|
|000|end|
|1xx|add|
|2xx|subtract|
|3xx|store|
|5xx|load|
|6xx|branch always|
|7xx|branch if 0|
|8xx|branch if >= 0|
|901|input|
|902|output|

## Example 1

### pseudo code (like python)

```python
temp = input()
print(temp + input())
exit(0)
```

### sj-assembly code

```
# Example 1
main:
    input
    store temp
    input
    add temp
    output
    end
data:
    temp = 0
```

## Example 1

### pseudo code (like python)

```python
temp = input()
if temp != 0:
    print(temp)
exit(0)
```

### sj-assembly code

```
# Example 2
main:
    input
    jz exit
    output
exit:
    end
```