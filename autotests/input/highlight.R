## R Script Sample File
## Source: http://www.rexamples.com

## Basics
a <- 42
A <- a * 2  # R is case sensitive
print(a)
cat(A, "\n") # "84" is concatenated with "\n"
if(A>a) # true, 84 > 42
{
  cat(A, ">", a, "\n")
}

## Functions
Square <- function(x) {
  return(x^2)
}

print(Square(4))
print(Square(x=4)) # same thing

DoSomething(color="red",number=55)

## Countdown
countdown <- function(from)
{
  print(from)
  while(from!=0)
  {
    Sys.sleep(1)
    from <- from - 1
    print(from)
  }
}

countdown(5)

## Reading user input
readinteger <- function()
{
  n <- readline(prompt="Enter an integer: ")
  n <- as.integer(n)
  if (is.na(n)){
    n <- readinteger()
  }
  return(n)
}
print(readinteger())

readinteger <- function()
{
  n <- readline(prompt="Enter an integer: ")
  if(!grepl("^[0-9]+$",n))
  {
    return(readinteger())
  }

  return(as.integer(n))
}
print(readinteger())

## Guess a random number game
readinteger <- function()
{
  n <- readline(prompt="Enter an integer: ")
  if(!grepl("^[0-9]+$",n))
  {
    return(readinteger())
  }
  return(as.integer(n))
}

# real program start here

num <- round(runif(1) * 100, digits = 0)
guess <- -1

cat("Guess a number between 0 and 100.\n")

while(guess != num)
{
  guess <- readinteger()
  if (guess == num)
  {
    cat("Congratulations,", num, "is right.\n")
  }
  else if (guess < num)
  {
    cat("It's bigger!\n")
  }
  else if(guess > num)
  {
    cat("It's smaller!\n")
  }
}

## Lists
sum(0:9)
append(LETTERS[1:13],letters[14:26])
c(1,6,4,9)*2
something <- c(1,4,letters[2])  # indices start at one, you get (1,4,"b")
length(something)

## Pipe
diamonds %>%
  filter(price > 1000) %>%
  group_by(cut) %>%
  tally() %>% 
  rename(tally = n) %>% 
  arrange(-tally) %>% 
  mutate(pct = tally / sum(tally)) -> df

## ggplot2 
plot = ggplot(diamonds, aes(x = price, y = carat)) +
  geom_point(alpha = 0.3, colour = 'steelblue') +
  labs(
    title = "ggplot diamonds",
    x = "Price, $",
    y = "Carat"
  ) +
  facet_wrap(~ cut) +
  theme_minimal()

plot + coord_flip()
