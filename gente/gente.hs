import System.Random

data Term = Var String
          | Abs String Term
          | App Term Term
          deriving (Eq, Show)

data TermEnum = EnumVar | EnumAbs | EnumApp deriving (Eq, Show, Enum)


-- pureGen = mkStdGen 42
-- (rnum, gen) = uniformR (0,2) pureGen
-- (rnum1, gen1) = uniformR (0,2) gen

mkTerm unif gen = do
    let (rn, g) = unif gen
    case rn of
        0 -> return (Var "x")
        1 -> return (Var "x")
        2 -> return (Var "x")

    
