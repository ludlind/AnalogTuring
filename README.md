A simulator of some simple models of analog turing machines.

# Model

Generally, the models implemented are fully continuous versions of the discrete Turing machine. They consist of a state $S(t)$, a memory tape $M(X,t)$ and a position along that tape $X(t)$. In the original Turing machine, a state transition consists of a discrete jump in the state variable $S$, followed by a memory write to M[X] where the symbol in the current position is set (possibly to the same value as before) and lastly, a step right or left in the tape position $X$.

The currently implemented models work with one-dimensional $S$, $X$ an $M$ varying
continuously in time according to a velocity function, $F(S, M[X])$ with
a component for $S$, $M$ at $X$ and $X$. However, this model turned out to be
too naive.

Currently, the goal is to implement a model that extends $S$ to two dimensions
(probably a complex number) and adds a phase variable $phi = \exp(i \omega t)$
such that the velocity function varies in time according to the phase, $F(S,
M[X], \phi)$.

# TODO

- [ ] Reorganize the code such that the state, solver, program and associated
  gui live in the same subdirectory to prepare for multimodel support
- [ ] Implement the new model
