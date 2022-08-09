# How to Contribute

## Git Branching Strategy

This project aims to keep a simple git history by having one long running branch (`main`) where branches are created, rebased onto, and then merged. Releases are simply tagged commits along the main branch.

When creating a branch, prefix the branch with the type of change being made:

- `feature/<feature-name>` for new features
- `bugfix/<bugfix-name>` for bux fixes

Prior to merging all branches should be rebased onto `main`, and ideally squashed into a single commit or a set of clear logical commits. After a merge, the branch will be deleted.

## Creating a Pull Request

- Pull requests should be done against the `main` branch.
- Pull requests should only have a single commit (with no merge commits) and should be rebased onto the latest commits on `main` when submitted.
- If adding new major features to the plugin, ensure appropriate tests have been added to the `BuildTimeLoggerTests` project that help verify the feature is working as intended.
- Provide a clear description of the purpose of your pull request. Is it a big fix, an additional feature? What is the rationale behind the work you have done? If you are planning on adding a large new feature, it is reccommended to open a ticket with the feature idea beforehand to ensure your intentions align with that of the maintence team and ensure your work has the best chance of being merged when done.

## Reporting Issues

Please feel free to post issues in the issues section of the repository. Please follow the template [here](https://gist.github.com/auremoser/72803ba969d0e61ff070) as a guide for how to format your issues.
