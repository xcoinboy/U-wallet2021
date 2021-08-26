**1. Clone wallet sources**

```
git clone https://github.com/seredat/U-wallet.git
```

**2. Set symbolic link to coin sources at the same level as `src`. For example:**

```
ln -s ../urbancash cryptonote
```

Alternative way is to create git submodule:

```
git submodule add https://gitlab.urbancash.org:445/urbancash/urbancash-cli.git cryptonote
```

**3. Build**

```
mkdir build && cd build && cmake .. && make
```
