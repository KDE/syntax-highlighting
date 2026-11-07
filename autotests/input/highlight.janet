# Adversarial Janet sample for Janet.xml validation.
# Every construct here is legal Janet 1.42.1; see README.md for the
# features the syntax definition deliberately approximates.

(def simple `one backtick`)
(def multi ``two backticks, may hold ` a single one``)
(def triple ```three backticks, may hold `` two```)
(def with-hash `# not a comment in here`)
(def not-an-unquote `raw ,not-unquote ~still-raw`)

(var state @{})
(set state :count ((fn [x] (+ x 1)) 1))
(def not-a-comment "# inside a string")

(def quoted ~(defn ^:private helper [x] ,(string x)))
(def spliced ~(array ,;(range 3)))

(def doubled (map |(* $ 2) [1 2 3]))
(def nested (map |(+ $ (get {:a 1} :a)) @[-1 0 1]))

# hash comment to end of line
# TODO alert words: FIXME BUG WARNING NOTE XXX HACK - rendered via the shared ##Alerts rules
(def hash-comment-test 1) # trailing hash comment with TODO marker
; semicolon comment to end of line
; FIXME alert words in the semicolon comment lane too: BUG and NOTE
(def semi-comment-test 2) ; trailing semicolon comment with FIXME marker

(def radix-binary 2r1011)
(def radix-base36 36rZZ)
(def radix-small 2r11)
(def hex-lower 0xdeadbeef)
(def hex-underscored 0xDEAD_BEEF)
(def underscored 1_000_000)
(def exponent 1e-3)
(def negative-exp -2.5E+10)
(def float 3.14)
(def negative -42)
(def plus-signed +17)

(def escapes "line\n tab\t return\r null\z bell\a esc\e quote\" slash\\")
(def escapes-hex "A\x42 a\x61")
(def escapes-unicode "u\u00e9 U\U0001F600")

(def vector @[1 2 3])
(def table @{:k :v})
(def array-of-arrays @[@[] @[:a :b]])
(def tuple-literal [1 2 3])
(def struct-literal {:a 1 :b 2})

(def keywords [:node :any :input :args :sides :alias :left :right])
(def ns-calls [string/join os/clock math/round peg/match ffi/write])

(defn labeled [xs]
  (label out (each x xs (when (> x 0) (return out)))))

(defn try-form [f]
  (try (f) ([err] (string "caught: " err))))

(def imported (import ./simple :as simple))

(defn short-fn-user [xs] (map |(* $ $) xs))

(defn matching [x]
  (match x
    {:type :point :x px :y py} (+ px py)
    [1 y] y
    _ nil))

(defn nested-longstring-trap []
  # The reader closes a longstring on a run of EXACTLY N backticks,
  # so two shorter runs do not end a two-backtick string:
  ``a ` b``)

(defn keywords-vs-symbols []
  (def :not-a-key 1)
  (def not-a-keyword 2)
  (def not_a_keyword 3)
  (def a-b-c 4))
