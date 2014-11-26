# Transform Milan

Follow progress here: https://trello.com/b/CZst5cii/transform-milan

## Stable
Master is current stable branch. Releases contain previous stable versions.

## Develop
Develop is the cutting edge branch. It's where we make changes and improvements.

## Fixing Bugs
### With Internet Access:

1. make a new branch called hotfix-# where # increments starting from 1.
2. Commit your changes to this branch.
3. Publish hotfix branch to origin. 
4. Then go to github, create a pull request (to master), merge into master, and delete branch. 
5. Finally pull down the changes from orgin master to local master in tower. 

### No Internet Access

1. Make a new branch called hotfix-# where # increments starting from 1.
2. Commit your changes to this branch.
3. Switch back to local master, and "merge" in changes from your hotfix branch using the merge button on tower. 
4. Push master to origin once you have internet access. 

## Project Dependencies

- https://github.com/julapy/ofxQuadWarp
- https://github.com/neilmendoza/ofxMovieExporter
- https://github.com/YCAMInterlab/ofxTimeline (you need to manually fix error which will throw on build)
