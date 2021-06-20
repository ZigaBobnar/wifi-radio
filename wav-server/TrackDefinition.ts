/**
 * Definition of track media file for loading purposes.
 */
class TrackDefinition {
  constructor(
    public path: string,
    public title?: string,
    public artist?: string
  ) {}
}

export { TrackDefinition };
