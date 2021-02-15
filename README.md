# Higher Kinded Sketch from this [tweet](https://twitter.com/fakenickels/status/1300869318033408001?s=20)

<blockquote class="twitter-tweet"><p lang="en" dir="ltr">Some experiments with lightweight higher kinded types in OCaml with <a href="https://twitter.com/reasonml?ref_src=twsrc%5Etfw">@reasonml</a> that I&#39;m using to grasp it better (not a proper blog post or tutorial at all). Probably once I grasp it better I&#39;ll do a better refined one.<a href="https://t.co/2YsbY3SLwQ">https://t.co/2YsbY3SLwQ</a> <a href="https://t.co/G8FyGJwWG5">pic.twitter.com/G8FyGJwWG5</a></p>&mdash; fakenickels (@fakenickels) <a href="https://twitter.com/fakenickels/status/1300869318033408001?ref_src=twsrc%5Etfw">September 1, 2020</a></blockquote> <script async src="https://platform.twitter.com/widgets.js" charset="utf-8"></script>


# Build + Watch

```bash
# for yarn
yarn start

# for npm
npm run start
```

# Sketch.sh Links Are Broken

I recovered this sketch with the excellent [letouzey/sketch-downloader](https://github.com/letouzey/sketch-downloader) by building and running:

Target sketch is: `https://sketch.sh/s/accmipAxAqXppOk30IIDa0/`

Below we are just using the hash,`accmipAxAqXppOk30IIDa0`.

```sh
> ./getsketch.sh accmipAxAqXppOk30IIDa0 dump.json
> dune exec ./transketch.exe dump.json
```

Thanks, [Letouzey](https://github.com/letouzey), whoever you are.
