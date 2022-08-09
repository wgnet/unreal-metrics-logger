# Maintainers

## Git Branching Policy

This project aims to keep a simple git history by having one long running branch (`main`) where branches are created, rebased onto, and then merged. Releases are simply tagged commits along the main branch.

When creating a branch, prefix the branch with the type of change being made:

- `feature/<feature-name>` for new features
- `bugfix/<bugfix-name>` for bux fixes

Prior to merging all branches should be rebased onto `main`, and ideally squashed into a single commit or a set of clear logical commits. After a merge, the branch should be deleted.

When there is a commit suitable for releasing, it should be tagged with the release version. The version released should match the source code in this tagged commit.

## Handling a Pull Request

- Assess whether the pull request is inline with the intended purpose of the extension.
- Ensure pull requests have been rebased onto the `main` branch prior to accepting the merge request.

## Creating a Release

N/A - currently this project is not released anywhere officially.
