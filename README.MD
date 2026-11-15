![](assets/20261115_100703_LOGO.PNG)

---

Finally - An XML runtime

XMLScripts is a programming language based on XML. It aims to be a turing complete framework to not just write programs entirely in XML, but makes itself easily embeddable into your C or C++ applications.

It is written in C, and should build on any platform or operating system.

This repository is discussed at length on my youtube channel:

[https://www.youtube.com/@ClosingTagStudios](https://www.youtube.com/@ClosingTagStudios)

### Example Code

```
<script>
<var v="1goi520" name="foo"/>
<function name="testFunction">
    <if lval="e" cond="equals" rval="e">
        <print msg="Hello from function 1."></print>
    </if>
</function>
<function name="testFunction2">
    <if lval="${foo}" cond="equals" rval="1goi520">
        <print msg="The comparison is true."></print>
    </if>
    <for var="${i}" start="0" end="10" step="1">
        <print msg="Current index is: ${i}" />
    </for>
</function>
<main>
    <set var="${foo}" v="5"/>
    <print msg="Foo equals ${foo}"/>
    <set var="${foo}" v="50"/>
    <print msg="Foo equals ${foo}"/>
    <testFunction inputVariable="5"/>
    <testFunction2/>
</main>
</script>
```


### Features

* [ ]  Variables
* [ ]  Arrays
* [ ]  Functions
* [ ]  Scope
* [ ]  Interface to embed into application
* [ ]  Separate examples from library
