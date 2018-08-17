# Swill Programming Language
<h2>Introduction</h2>
The Swill programming language project began early July, 2018, as a half-hearted attempt at language design. But now, it has become a serious project with bright prospects. At the moment, Swill is a very small objected oriented computer programming language. It has most basic language features to allow the creation of simple programs.

<h2>A Taste Of Swill</h2>
<pre>
// The Start model is the program's
// entry point
model Start{
  __exec{
    Person person("Sunday Etom", 50);
    person.printInfo();
  }
}

model Person{
 public
 __decl{


  function printInfo(){
    println("Name: "+name);
    println("Age : "+age);
  }
 }
 
 private
 __decl{
   string name;
   num age;
 }
 
 __ctor(name, age){
    // __ctor does nothing more than
    // assigning given values to its arguments
 }
}
</pre>
