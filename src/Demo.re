/* HKT with Lightweight Higher Kinded Type */
/* ======================================= */

/* fetched from https://sketch.sh/s/accmipAxAqXppOk30IIDa0 */

/*:
TL;DR; explanation of what the hell are _higher kinded types_: they allow the parametrization of parametric types. Which is currently not possible in OCaml and of course Reason.

But there is a hack called Lightweight Higher Kinded Type described in http://ocamllabs.io/higher/lightweight-higher-kinded-polymorphism.pdf with a demo implementation available at https://github.com/ocamllabs/higher/blob/master/src/higher.ml

Experiments with a poetic license for brevity.
:*/
/*:
## Definiting the boilerplate for HKT with modules
:*/
/* apply */
type app('p, 'f);

module type Newtype1 = {
  /* this will reference the original type that should be made a HKT
   the 'a is important because it's how we are going to separate the type parameter
   from the "function" type
  */
  type s('a);

  /* uninhabited opaque type to be our "applyable" type */
  type t;

  let bind: ('a => 'b, s('a)) => s('b);
  /* inject */
  external inj: s('a) => app('a, t) = "%identity";
  /* project */
  external prj: app('a, t) => s('a) = "%identity";
};
module Common = {
  type t;
  external inj: 'a => 'b = "%identity";
  external prj: 'a => 'b = "%identity";
};
module Newtype1 =
       (
         T: {
           type t('a);
           let ret: 'a => t('a);
           let bind: ('a => 'b, t('a)) => t('b);
         },
       ) => {
  type s('a) = T.t('a);
  let ret = T.ret;
  let bind = T.bind;
  include Common;
};


/*:
With our types defined we can try to instantiate some juicy classes
:*/
module OptionFunctor: Newtype1 with type s('a) = option('a) =
  Newtype1({
    type t('a) = option('a);
    let ret = a => Some(a);
    let bind = (f, a) =>
      switch (a) {
      | Some(value) => Some(f(value))
      | None => None
      };
  });

module ListFunctor: Newtype1 with type s('a) = list('a) =
  Newtype1({
    type t('a) = list('a);
    let ret = a => [a];
    let bind = (f, a) =>
      switch (a) {
      | [] => []
      | values => List.map(f, values)
      };
  });
/*:
We can define a `map` function to rule them all
:*/
let map =
    (
      type f,
      module Functor: Newtype1 with type t = f,
      f: 'a => 'b,
      /* Levaring the HKT here, we can say we want something of the type 'f('a) or `f a` */
      hfa: app('a, f),
    ) =>
  Functor.bind(value => f(value), Functor.prj(hfa)) |> Functor.inj;

/*:
Now let's give those bad bois a ~~slap~~ try out
:*/
let user = OptionFunctor.inj(Some("Henlou"));
let myNumberBois = ListFunctor.inj([1, 2, 3, 5]);

let _ = (user |> map((module OptionFunctor), a => "mr. " ++ a));

let _ = (myNumberBois |> map((module ListFunctor), a => a + 12));
/*:
Ok alright, we are actually mixing HKT with some [`modular explicits-ish`](https://tycon.github.io/modular-implicits.html) in there.
:*/
/*:
# Type inference check
Wrong map function
:*/
/* (myNumberBois |> map((module ListFunctor), a => a ++ "12")); */
/*:
Wrong functor context
:*/
/* (myNumberBois |> map((module OptionFunctor), a => a + 12)); */
/*:
# Other usages
Let's see how the `Higher` technique works with GADTs like in `lenses-ppx`
:*/
module StateLenses = {
  type state = {
    email: string,
    age: int,
  };
  type field(_) =
    | Email: field(string)
    | Age: field(int);
  let get: type value. (state, field(value)) => value =
    (state, field) =>
      switch (field) {
      | Email => state.email
      | Age => state.age
      };
  let set: type value. (state, field(value), value) => state =
    (state, field, value) =>
      switch (field) {
      | Email => {...state, email: value}
      | Age => {...state, age: value}
      };
};

module type LensesHKT = {
  type s('a);
  type t;
  type state;
  let get: (state, s('a)) => 'a
  let set: (state, s('a), 'a) => state;
  external inj: s('a) => app('a, t) = "%identity";
  external prj: app('a, t) => s('a) = "%identity";
};

module LensesHKT =
       (
         T: {
           type field('a);
          type state;
          let get: (state, field('a)) => 'a
          let set: (state, field('a), 'a) => state;
         },
       ): (LensesHKT with type s('a) = T.field('a) and type state = T.state) => {
  type s('a) = T.field('a);
  type state = T.state;
  let set = T.set;
  let get = T.get;
  include Common;
};
module StateLensesHKT  = LensesHKT(StateLenses)
let form = StateLenses.{
 email: "fakenickels@gov.br",
 age: 12
};

let _ = StateLensesHKT.get(form, Email)
type validator('field) =
  | Email(app(string, 'field)): validator('field)
  | IntMax(int, app(int, 'field)): validator('field)
  | IntMin(int, app(int, 'field)): validator('field);

let validateField = (type f, type s, module Lenses: LensesHKT with type t = f and type state = s, validator: validator(f), state: s) => {
 switch(validator) {
   | Email(field) =>
     let field = Lenses.prj(field);
     String.length(Lenses.get(state, field)) > 10 ? Ok() : Error("Invalid email")
   | IntMax(max, field) =>
     let field = Lenses.prj(field);
     let value = Lenses.get(state, field)
     value < max ? Ok() : Error(string_of_int(value) ++ " is greater than " ++ string_of_int(max))
   | IntMin(min, field) =>
     let field = Lenses.prj(field);
     let value = Lenses.get(state, field)
     value > min ? Ok() : Error(string_of_int(value) ++ " is less than " ++ string_of_int(min))
 }
}
let _ = validateField((module StateLensesHKT), IntMax(10, StateLensesHKT.inj(StateLenses.Age)), form)
let _ = validateField((module StateLensesHKT), IntMax(18, StateLensesHKT.inj(StateLenses.Age)), form)
/*:
But will it type check?
:*/
/* validateField((module StateLensesHKT), IntMax(10, StateLensesHKT.inj(StateLenses.Email)), form) */

let intMax = (type field, module Lenses: LensesHKT with type t = field, ~max, field: app(int, field)) =>
 IntMax(max, field)
intMax((module StateLensesHKT), ~max=18, StateLensesHKT.inj(StateLenses.Age))
/* type checks! */
/* intMax((module StateLensesHKT), ~max=18, StateLensesHKT.inj(StateLenses.Email)) */
let validate = (type field, type state, module Lenses: LensesHKT with type t = field and type state = state, schema: list(validator(Lenses.t)), state: state) => {
 List.map(validator => {
  validateField((module Lenses), validator, state)
 }, schema)
}
let schema = StateLensesHKT.[
  IntMax(10, inj(StateLenses.Age)),
  Email(inj(StateLenses.Email))
]

validate((module StateLensesHKT), schema, form)
/*:
We can't do a function like `intMax((module StateLensesHKT), ~max=18, StateLenses.Age)` because that's the whole point of the HKT hack
:*/
/*:
# Final consideration
Just use the goddamn ML functors
:*/

