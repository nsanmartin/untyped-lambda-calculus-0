data Term = Var String
          | Abs String Term
          | App Term Term
          deriving (Eq, Show)

data TermEnum = EnumVar | EnumAbs | EnumApp deriving (Eq, Show, Enum)




-- Basic definitions

newtype LamGen = Lg [TermEnum] -> [(Term, [TermEnum])])

genLam :: LamGen -> [TermEnum] -> [(Term, [TermEnum])]
genLam (Lg g) ing = g ing

item :: LamGen
item = Lg (\ing -> case ing of
                     []     -> []
                     (x:xs) -> [(Var "x", xs)])



-- Sequencing generators

instance Functor LanGen where
   -- fmap :: (a -> b) -> LanGen -> LanGen
   fmap f g = P (\inp -> case genLam g ing of
                            []        -> []
                            [(v,out)] -> [(f v, out)])

instance Applicative Parser where
   -- pure :: a -> LamGen
   genLam v = Lg (\inp -> [(v,inp)])

   -- <*> :: Parser (a -> b) -> Parser a -> Parser b
   pg <*> px = P (\inp -> case parse pg inp of
                             []        -> []
                             [(g,out)] -> parse (fmap g px) out)

instance Monad Parser where
   -- (>>=) :: Parser a -> (a -> Parser b) -> Parser b
   p >>= f = P (\inp -> case parse p inp of
                           []        -> []
                           [(v,out)] -> parse (f v) out)

