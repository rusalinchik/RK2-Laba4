CREATE INDEX idx_students_group_number ON students(group_number);
CREATE INDEX idx_grades_student_id ON grades(student_id);
CREATE INDEX idx_grades_subject_id ON grades(subject_id);

ALTER TABLE notes ADD COLUMN IF NOT EXISTS note_text_tsvector TSVECTOR;

UPDATE notes SET note_text_tsvector = to_tsvector('russian', note_text);

CREATE INDEX idx_notes_note_text_gin ON notes USING GIN(note_text_tsvector);

CREATE OR REPLACE FUNCTION notes_tsvector_update() RETURNS TRIGGER AS $$
BEGIN
    NEW.note_text_tsvector = to_tsvector('russian', NEW.note_text);
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER notes_tsvector_update_trigger
    BEFORE INSERT OR UPDATE ON notes
    FOR EACH ROW
    EXECUTE FUNCTION notes_tsvector_update();